#include "Force.hpp"

#include "ECS/ECS.hpp"

Error ForceSystem(World &GameWorld, DSeconds dt)
{
	for (size_t Entity = 0; Entity < GameWorld.size(); Entity++)
	{
		if (GameWorld[Entity].Force())
		{
			bool Updated = false;
			for (auto &Force : *GameWorld[Entity].Force())
			{
				glm::dvec3 ToPosition
				    = Force.Force; // no magic center of gravity stuff yet
				glm::dquat Rotation;
				if (GameWorld[Entity].Position())
				{
					(*GameWorld[Entity].Position()) += ToPosition;
				}
				if (ToPosition != glm::dvec3{})
				{
					Updated = true;
				}
			}
			if (Updated)
			{
				GameWorld.UpdatedEntities.push_back(Entity);
			}
			GameWorld[Entity].Force()->clear();
		}
	}
}