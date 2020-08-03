#include "Children.hpp"

#include "ECS/ECS.hpp"

Error Children::EnforceCorrectness(World &GameWorld, size_t Me)
{
	if (Parent != -1)
	{
		if (!GameWorld[Parent].Children())
		{
			GameWorld[Parent].Children() = ::Children{};
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
			GameWorld[ChildID].Children() = ::Children{};
		}
		auto &Child = *GameWorld[ChildID].Children();
		Child.Parent = Me;
	}
	return Error(Error::Type::None);
}

glm::dmat4x4 Children::CalculateFullTransform(
    const World &GameWorld,
    size_t Me,
    bool UseBackup) const
{
	glm::dmat4x4 Parent = CalculateParentTransform(GameWorld, UseBackup);
	if (GameWorld[Me].Transform() && !UseBackup)
	{

		return Parent * GameWorld[Me].Transform()->CalculateFull(GameWorld, Me, UseBackup);
	}
	else if (
	    UseBackup && GameWorld[Me].BasicBackup()
	    && GameWorld[Me].BasicBackup()->Transform_)
	{
		return Parent
		       * GameWorld[Me].BasicBackup()->Transform_->CalculateFull(GameWorld, Me, UseBackup);
	}
	else
	{
		return Parent;
	}
}

glm::dmat4x4
Children::CalculateParentTransform(const World &GameWorld, bool UseBackup) const
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
	    Parent,
	    UseBackup);
}
