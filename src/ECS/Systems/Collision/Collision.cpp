#include "Collision.hpp"

#include <functional>
#include <string>

#include "ECS/ECS.hpp"
#include "ECS/TransformedMinMax.hpp"
#include "minitrace/minitrace.h"

#include "triangles.c"

Error UpdateOctree(World &GameWorld, DSeconds dt)
{
	for (auto &Entity : GameWorld.UpdatedEntities)
	{
		GameWorld.CollisionOctree.Update(Entity);
	}
	GameWorld.UpdatedEntities.clear();

	return {Error::Type::None};
}

bool IndevidualCollision(Meshes &Mesh1, Meshes &Mesh2, glm::dmat4 ToSecond)
{
	MTR_SCOPE("Main", __PRETTY_FUNCTION__, 0);

	{
		MTR_SCOPE("Main", "AABB", 0.000001);

		auto [Min1, Max1] = CalculateTransformedMinMax(Mesh1, ToSecond);
		auto [Min2, Max2] = CalculateTransformedMinMax(Mesh2, glm::dmat2{1});

		auto XCase = (Min1.x <= Min2.x && Max1.x >= Min2.x) ||
		             (Max1.x >= Max2.x && Min1.x <= Max2.x) ||
		             (Min1.x <= Min2.x && Max1.x >= Max2.x);

		auto YCase = XCase || (Min1.y <= Min2.y && Max1.y >= Min2.y) ||
		             (Max1.y >= Max2.y && Min1.y <= Max2.y) ||
		             (Min1.y <= Min2.y && Max1.y >= Max2.y);

		auto ZCase = YCase || (Min1.z <= Min2.z && Max1.z >= Min2.z) ||
		             (Max1.z >= Max2.z && Min1.z <= Max2.z) ||
		             (Min1.z <= Min2.z && Max1.z >= Max2.z);

		if (!ZCase)
		{
			return ZCase;
		}
	}

	std::reference_wrapper<const std::vector<glm::dvec3>> Temp1{
	    Meshes::StaticMeshes[Mesh1.MeshIndex].Triangles()};
	if (Mesh1.Type != Meshes::MeshType::Static)
	{
		Temp1 = std::ref(Meshes::TexturedMeshes[Mesh1.MeshIndex].Triangles());
	}
	auto Triangles = Temp1.get();

	std::reference_wrapper<std::optional<Octree>> Temp2{
	    Meshes::StaticOctrees[Mesh2.MeshIndex]};
	if (Mesh1.Type != Meshes::MeshType::Static)
	{
		Temp2 = std::ref(Meshes::TexturedOctrees[Mesh1.MeshIndex]);
	}
	auto Octree = Temp2.get();
	if (!Octree)
	{
		return false;
	}
	std::reference_wrapper<const std::vector<glm::dvec3>> Temp3{
	    Meshes::StaticMeshes[Mesh1.MeshIndex].Triangles()};
	if (Mesh1.Type != Meshes::MeshType::Static)
	{
		Temp3 = std::ref(Meshes::TexturedMeshes[Mesh1.MeshIndex].Triangles());
	}
	auto Triangles2 = Temp3.get();
	for (size_t i = 0; i < Triangles.size() / 3; i++)
	{
		Meshes::Custom = {
		    ToSecond * glm::dvec4(Triangles[i * 3], 1),
		    ToSecond * glm::dvec4(Triangles[i * 3 + 1], 1),
		    ToSecond * glm::dvec4(Triangles[i * 3 + 2], 1)};
		auto PossibleCollisions = Octree->GetColliding(size_t(-1));
		for (auto &OtherTriangle : PossibleCollisions)
		{
			auto Intersects = tri_tri_overlap_test_3d(
			    glm::value_ptr(Meshes::Custom[0]),
			    glm::value_ptr(Meshes::Custom[1]),
			    glm::value_ptr(Meshes::Custom[2]),
			    glm::value_ptr(Triangles2[OtherTriangle * 3]),
			    glm::value_ptr(Triangles2[OtherTriangle * 3 + 1]),
			    glm::value_ptr(Triangles2[OtherTriangle * 3 + 2]));
			if (Intersects == 1)
			{
				return true;
			}
		}
	}
	return false;
}

Error HandleCollisions(World &GameWorld, DSeconds dt)
{
	for (size_t Entity = 0; Entity < GameWorld.size(); Entity++)
	{
		if (GameWorld[Entity].Mesh())
		{
			glm::dmat4 Transform{1};
			if (GameWorld[Entity].Children())
			{
				Transform =
				    GameWorld[Entity].Children()->CalculateFullTransform(
				        GameWorld,
				        Entity);
			}
			else if (GameWorld[Entity].Transform())
			{
				Transform = GameWorld[Entity].Transform()->CalculateFull();
			}
			auto PossiblyColliginEntities =
			    GameWorld.CollisionOctree.GetColliding(Entity);
			for (auto &Colliding : PossiblyColliginEntities)
			{
				if (Colliding == Entity)
				{
					continue;
				}
				if (GameWorld[Colliding].Mesh())
				{
					glm::dmat4 CollidingTransform{1};
					if (GameWorld[Colliding].Children())
					{
						CollidingTransform =
						    GameWorld[Colliding]
						        .Children()
						        ->CalculateFullTransform(GameWorld, Colliding);
					}
					else if (GameWorld[Colliding].Transform())
					{
						CollidingTransform =
						    GameWorld[Colliding].Transform()->CalculateFull();
					}
					CollidingTransform =
					    glm::inverse(CollidingTransform) * Transform;
					auto Result = IndevidualCollision(
					    *GameWorld[Entity].Mesh(),
					    *GameWorld[Colliding].Mesh(),
					    CollidingTransform);
					if (Result)
					{
						if (GameWorld[Entity].BasicBackup())
						{
							GameWorld[Entity].Position() =
							    GameWorld[Entity].BasicBackup()->Position;
							GameWorld[Entity].Transform() =
							    GameWorld[Entity].BasicBackup()->Transform_;
							GameWorld.UpdatedEntities.push_back(Entity);
						}
						break;
					}
				}
			}
		}
	}
	return {Error::Type::None};
}