#include "BasicBackup.hpp"

#include "ECS/ECS.hpp"

Error BasicBackupSystem(World &GameWorld, DSeconds dt)
{
	for(size_t i=0; i<GameWorld.size(); i++)
	{
		if(GameWorld[i].BasicBackup())
		{
			GameWorld[i].BasicBackup()->Position = GameWorld[i].Position();
			GameWorld[i].BasicBackup()->Transform_ = GameWorld[i].Transform();
		}
	}
	return {Error::Type::None};
}