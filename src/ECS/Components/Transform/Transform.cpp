#include "Transform.hpp"

#include "ECS/ECS.hpp"

glm::dmat4x4 Transform::CalculateFull(const World &GameWorld, size_t id, bool UseBackup) const
	{
		glm::dmat4x4 Result(1);
		for (auto &Matrix : Tranformations)
		{
			if (Matrix.first == Type::AutoPosition)
			{
				if (GameWorld[id].Position() && !UseBackup)
				{
					Result = glm::translate(
					             glm::dmat4{1},
					             *GameWorld[id].Position())
					         * Result;
				}
				else if(GameWorld[id].BasicBackup()->Position && UseBackup)
				{
					Result = glm::translate(
					             glm::dmat4{1},
					             *GameWorld[id].BasicBackup()->Position)
					         * Result;
				}
			}
			else
			{
				Result = Matrix.second * Result;
			}
		}
		return Result;
	}