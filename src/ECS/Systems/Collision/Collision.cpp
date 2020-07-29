#include "Collision.hpp"

#include "ECS/ECS.hpp"

Error UpdateOctree(World &GameWorld, DSeconds dt)
{
	for(auto &Entity : GameWorld.UpdatedEntities)
	{
		GameWorld.CollisionOctree.Update(Entity);
	}
	GameWorld.UpdatedEntities.clear();
}


Error HandleCollisions(World &GameWorld, DSeconds dt) {}