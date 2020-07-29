#include "Children.hpp"

#include "ECS/ECS.hpp"

Error Children::EnforceCorrectness(World &GameWorld, size_t Me)
{
	if (Parent != -1)
	{
		if (!GameWorld[Parent].Children())
		{
			ActivateComponent<World::Children>(
			    static_cast<size_t>(Parent),
			    GameWorld);
		}
		auto &MyParent = *GameWorld[Parent].Children();
		bool MeInParent = false;
		for (size_t i = 0; i < MyParent.Children.size(); i++)
		{
			if (MyParent.Children[i] == Me)
			{
				if (MeInParent == false)
				{
					MeInParent = true;
					continue;
				}
				else
				{
					MyParent.Children.erase(MyParent.Children.begin() + i);
					i--;
				}
			}
		}
		if (MeInParent == false)
		{
			MyParent.Children.push_back(Me);
		}
	}
	for (auto &ChildID : Children)
	{
		if (!GameWorld[ChildID].Children())
		{
			ActivateComponent<World::Children>(ChildID, GameWorld);
		}
		auto &Child = *GameWorld[ChildID].Children();
		Child.Parent = Me;
	}
	return Error(Error::Type::None);
}

glm::dmat4x4 Children::CalculateFullTransform(World &GameWorld, size_t Me)
{
	glm::dmat4x4 Parent = CalculateParentTransform(GameWorld);
	if (GameWorld[Me].Transform())
	{
		return Parent * GameWorld[Me].Transform()->CalculateFull();
	}
	else
	{
		return Parent;
	}
}

glm::dmat4x4 Children::CalculateParentTransform(World &GameWorld)
{
	if (Parent == -1)
	{
		return glm::dmat4x4(1);
	}
	if (!GameWorld[Parent].Children())
	{
		return glm::dmat4x4(1);
	}
	return GameWorld[Parent].Children()->CalculateFullTransform(
	    GameWorld,
	    Parent);
}