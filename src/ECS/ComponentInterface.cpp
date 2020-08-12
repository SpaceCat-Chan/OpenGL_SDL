#include "ComponentInterface.hpp"

#include "ECS.hpp"

glm::dmat4 CalcTransform(const World &GameWorld, size_t id, bool UseBackup)
{
	if (GameWorld[id].Children())
	{
		return GameWorld[id].Children()->CalculateFullTransform(
		    GameWorld,
		    id,
		    UseBackup);
	}
	else if (GameWorld[id].Transform() && !UseBackup)
	{
		return GameWorld[id].Transform()->CalculateFull(
		    GameWorld,
		    id,
		    UseBackup);
	}
	else if (GameWorld[id].BasicBackup() && UseBackup)
	{
		if (GameWorld[id].BasicBackup()->Transform_)
		{
			return GameWorld[id].BasicBackup()->Transform_->CalculateFull(
			    GameWorld,
			    id,
			    UseBackup);
		}
		else
		{
			return glm::dmat4{1};
		}
	}
	else
	{
		return glm::dmat4{1};
	}
}

glm::dmat4
CalcCollisionTransform(const World &GameWorld, size_t id, bool UseBackup)
{
	return CalcTransform(GameWorld, id, UseBackup)
	       * CalcCollisionModelTransform(GameWorld, id);
}

glm::dmat4 CalcCollisionModelTransform(const World &GameWorld, size_t id)
{
	if(GameWorld[id].Collision() && GameWorld[id].Collision()->CollisionMesh)
	{
		return GameWorld[id].Collision()->CollisionMeshToModelSpace;
	}
	else
	{
		return glm::dmat4{1};
	}
	
}