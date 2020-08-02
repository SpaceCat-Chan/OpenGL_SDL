#include "AutoPosition.hpp"

#include "ECS/ECS.hpp"

Error AutoPositionSystem(World &GameWorld, DSeconds dt)
{
	for (size_t i = 0; i < GameWorld.size(); i++)
	{
		if (GameWorld[i].Transform())
		{
			auto &Transform = *GameWorld[i].Transform();
			auto &Position = GameWorld[i].Position();
			if (Transform.PositionComponentSpace == Transform::Space::View)
			{
				bool HitAutoPosition = false;
				for (auto &Matrix : Transform.Tranformations)
				{
					if (Matrix.first == Transform::Type::AutoPosition)
					{
						if (HitAutoPosition == false && Position)
						{
							Matrix.second =
							    glm::translate(glm::dmat4x4(1), *Position);
							HitAutoPosition = true;
						}
						else
						{
							Matrix.second = glm::dmat4x4(1);
						}
					}
				}
			}
		}
	}
	return Error(Error::Type::None);
}