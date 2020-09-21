#include "Collision.hpp"

#include <functional>
#include <string>

#include "ECS/ComponentInterface.hpp"
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

		auto XCase = (Min1.x <= Min2.x && Max1.x >= Min2.x)
		             || (Max1.x >= Max2.x && Min1.x <= Max2.x)
		             || (Min1.x <= Min2.x && Max1.x >= Max2.x);

		auto YCase = XCase || (Min1.y <= Min2.y && Max1.y >= Min2.y)
		             || (Max1.y >= Max2.y && Min1.y <= Max2.y)
		             || (Min1.y <= Min2.y && Max1.y >= Max2.y);

		auto ZCase = YCase || (Min1.z <= Min2.z && Max1.z >= Min2.z)
		             || (Max1.z >= Max2.z && Min1.z <= Max2.z)
		             || (Min1.z <= Min2.z && Max1.z >= Max2.z);

		if (!ZCase)
		{
			return ZCase;
		}
	}

	const std::vector<glm::dvec3> &Mesh1Triangles = Mesh1.Triangles();
	const std::vector<glm::dvec3> &Mesh2Triangles = Mesh2.Triangles();

	std::optional<Octree> &Mesh2Octree = Mesh2.MeshOctree();
	if (!Mesh2Octree)
	{
		return false;
	}

	for (size_t i = 0; i < Mesh1Triangles.size() / 3; i++)
	{
		Meshes::Custom
		    = {ToSecond * glm::dvec4(Mesh1Triangles[i * 3], 1),
		       ToSecond * glm::dvec4(Mesh1Triangles[i * 3 + 1], 1),
		       ToSecond * glm::dvec4(Mesh1Triangles[i * 3 + 2], 1)};

		std::unordered_set<size_t> PossibleCollisions
		    = Mesh2Octree->GetColliding(size_t(-1));
		for (auto &OtherTriangle : PossibleCollisions)
		{
			int Intersects = tri_tri_overlap_test_3d(
			    glm::value_ptr(Meshes::Custom[0]),
			    glm::value_ptr(Meshes::Custom[1]),
			    glm::value_ptr(Meshes::Custom[2]),
			    glm::value_ptr(Mesh2Triangles[OtherTriangle * 3]),
			    glm::value_ptr(Mesh2Triangles[OtherTriangle * 3 + 1]),
			    glm::value_ptr(Mesh2Triangles[OtherTriangle * 3 + 2]));
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
			glm::dmat4 Transform = CalcCollisionTransform(GameWorld, Entity);

			auto PossiblyColliginEntities
			    = GameWorld.CollisionOctree.GetColliding(Entity);
			for (auto &Colliding : PossiblyColliginEntities)
			{
				if (Colliding == Entity)
				{
					continue;
				}
				if (GameWorld[Colliding].Mesh())
				{
					glm::dmat4 CollidingTransform
					    = CalcCollisionTransform(GameWorld, Colliding);

					glm::dmat4 EntityToCollidingTransform
					    = glm::inverse(CollidingTransform) * Transform;

					Meshes EntityMesh = CollisionMesh(GameWorld, Entity);
					Meshes CollidingMesh = CollisionMesh(GameWorld, Colliding);

					bool Result = IndevidualCollision(
					    EntityMesh,
					    CollidingMesh,
					    EntityToCollidingTransform);

					if (Result)
					{
						if (GameWorld[Entity].BasicBackup())
						{
							GameWorld[Entity].Position()
							    = GameWorld[Entity].BasicBackup()->Position;
							GameWorld[Entity].Transform()
							    = GameWorld[Entity].BasicBackup()->Transform_;
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