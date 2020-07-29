#include "Octree.hpp"

#include <iostream>

uint8_t
CalcSection(glm::dvec3 Center, double Radius, std::array<glm::dvec3, 2> AABB)
{
	if (AABB[0].x < Center.x - Radius || AABB[1].x > Center.x + Radius ||
	    AABB[0].y < Center.y - Radius || AABB[1].y > Center.y + Radius ||
	    AABB[0].z < Center.z - Radius || AABB[1].z > Center.z + Radius)
	{
		return Detail::OutOfBounds;
	}
	if ((AABB[0].x < Center.x && AABB[1].x > Center.x) ||
	    (AABB[0].y < Center.y && AABB[1].y > Center.y) ||
	    (AABB[0].z < Center.z && AABB[1].z > Center.z))
	{
		return Detail::OverCenter;
	}
	uint8_t Result = 0b000;
	if (AABB[0].x > Center.x)
	{
		Result |= Detail::X;
	}
	if (AABB[0].y > Center.y)
	{
		Result |= Detail::Y;
	}
	if (AABB[0].z > Center.z)
	{
		Result |= Detail::Z;
	}
	return Result;
}

void Detail::Octree_Impl::RecieveFromChild(size_t id, uint8_t Section)
{
	auto Where = CalcSection(m_Center, m_Radius, m_GetAABB(id));

	if (Where == Detail::OutOfBounds)
	{
		if (m_Parent)
		{
			m_Parent->RecieveFromChild(id, m_Section);
		}
		else
		{
			m_TopLevel->OutOfBounds(id);
		}
	}
	else if (Where == Detail::OverCenter)
	{

		m_Content.insert(id);
	}
	else
	{
		if (Where == Section)
		{
			std::cerr << "child octree went crazy: " << Where << ", " << id
			          << '\n';
			assert(false);
		}
		else if (m_Children[Where] == nullptr)
		{
			m_Children[Where] =
			    std::make_unique<Detail::Octree_Impl>(m_GetAABB, this, Where);
		}
		m_Children[Where]->Add(id);
	}
}

void Detail::Octree_Impl::Add(size_t id)
{
	auto Where = CalcSection(m_Center, m_Radius, m_GetAABB(id));
	if (Where == Detail::OutOfBounds)
	{
		if (m_Parent)
		{
			std::cerr << "parent went crazy: " << Where << ", " << id << '\n';
			assert(false);
		}
		else
		{
			m_TopLevel->OutOfBounds(id);
		}
	}
	else if (Where == Detail::OverCenter || MaxBranchingReached())
	{
		m_Content.insert(id);
	}
	else
	{
		if (m_Children[Where] == nullptr)
		{
			m_Children[Where] =
			    std::make_unique<Detail::Octree_Impl>(m_GetAABB, this, Where);
		}
		m_Children[Where]->Add(id);
	}
}

bool Detail::Octree_Impl::Update(size_t id)
{
	auto Found = m_Content.find(id);
	if (Found != m_Content.end())
	{
		auto Where = CalcSection(m_Center, m_Radius, m_GetAABB(id));
		if (Where == Detail::OutOfBounds)
		{
			if (m_Parent)
			{
				m_Parent->RecieveFromChild(id, m_Section);
			}
			else
			{
				m_TopLevel->OutOfBounds(id);
			}
		}
		else if (Where != Detail::OverCenter)
		{
			if (m_Children[Where] == nullptr)
			{
				m_Children[Where] = std::make_unique<Detail::Octree_Impl>(
				    m_GetAABB,
				    this,
				    Where);
			}
			m_Children[Where]->Add(id);
		}
		m_Content.erase(Found);
		return true;
	}
	else
	{
		for (auto &child : m_Children)
		{
			if (child)
			{
				if (child->Update(id))
				{
					return true;
				}
			}
		}
		return false;
	}
}

void Detail::Octree_Impl::Remove(size_t id)
{
	auto Where = CalcSection(m_Center, m_Radius, m_GetAABB(id));
	if (Where == Detail::OutOfBounds)
	{
		if (m_Parent)
		{
			std::cerr << "Octree::Remove: parent went crazy: " << id << '\n';
		}
	}
	else if (Where == Detail::OverCenter)
	{
		auto Found = m_Content.find(id);
		if (Found != m_Content.end())
		{
			m_Content.erase(Found);
		}
	}
	else
	{
		if (m_Children[Where] != nullptr)
		{
			m_Children[Where]->Remove(id);
		}
	}
}

void Octree::Add(size_t id) { m_Root->Add(id); }

void Octree::Update(size_t id) { m_Root->Update(id); }

void Octree::Remove(size_t id) { m_Root->Remove(id); }

void Octree::OutOfBounds(size_t id)
{
	auto CurrentRadius = m_Root->m_Radius;
	auto CurrentCenter = m_Root->m_Center;
	auto AABB = m_GetAABB(id);

	auto AveragePosition = (AABB[0] + AABB[1]) / 2.0;
	auto Diff = AveragePosition - CurrentCenter;
	glm::dvec3 Sign = glm::sign(Diff);

	for (auto &xyz : Sign)
	{
		if (xyz == 0)
		{
			xyz = 1;
		}
	}

	auto NewCenter = CurrentCenter + Sign * CurrentRadius;

	auto NewRoot = std::make_unique<Detail::Octree_Impl>(m_GetAABB);
	NewRoot->m_Center = std::move(NewCenter);
	NewRoot->m_Radius = CurrentRadius * 2;

	for (auto &xyz : Sign)
	{
		if (xyz == -1)
		{
			xyz = 0;
		}
	}

	m_Root->m_Section = int(Sign.x) | int(Sign.y) << 1 | int(Sign.z) << 2;

	m_Root->m_Parent = NewRoot.get();
	NewRoot->m_Children[m_Root->m_Section] = std::move(m_Root);
	m_Root = std::move(NewRoot);
	m_Root->Add(id);
}