#pragma once

#include <array>
#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

class Octree;
namespace Detail
{
enum Directions
{
	X = 0b00001,
	Y = 0b00010,
	Z = 0b00100,
	OverCenter = 0b01000,
	OutOfBounds = 0b10000
};
class Octree_Impl
{
	friend class ::Octree;
	Octree_Impl *m_Parent = nullptr;
	Octree *m_TopLevel = nullptr;
	std::array<std::unique_ptr<Octree_Impl>, 8> m_Children = {nullptr};
	std::function<std::array<glm::dvec3, 2>(size_t)> m_GetAABB;
	uint8_t m_Section = 0;

	glm::dvec3 m_Center = {0, 0, 0};
	double m_Radius = 1000;

	std::unordered_set<size_t> m_Content;


	bool MaxBranchingReached() { return m_Radius < 0.001; }

	void RecieveFromChild(size_t id, uint8_t Section);

	public:
	Octree_Impl(
	    decltype(m_GetAABB) GetAABB,
	    Octree_Impl *Parent,
	    uint8_t Section)
	    : m_Parent(Parent), m_GetAABB(GetAABB), m_Section(Section)
	{
		m_Radius = Parent->m_Radius / 2;

		m_Center = Parent->m_Center;
		m_Center.x += m_Radius * ((X & Section) ? 1.0 : -1.0);
		m_Center.y += m_Radius * ((Y & Section) ? 1.0 : -1.0);
		m_Center.z += m_Radius * ((Z & Section) ? 1.0 : -1.0);
	}
	Octree_Impl(decltype(m_GetAABB) GetAABB) : m_GetAABB(GetAABB) {}

	Octree_Impl(const Octree_Impl &copy)
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (copy.m_Children[i])
			{
				m_Children[i] =
				    std::make_unique<Octree_Impl>(*copy.m_Children[i]);
				m_Children[i]->m_Parent = this;
			}
		}
		m_GetAABB = copy.m_GetAABB;
		m_Section = copy.m_Section;
		m_Content = copy.m_Content;

		m_Center = copy.m_Center;
		m_Radius = copy.m_Radius;
	}
	Octree_Impl(Octree_Impl &&move)
	{
		m_Parent = move.m_Parent;
		m_TopLevel = move.m_TopLevel;
		m_Children = std::move(move.m_Children);
		for (auto &child : m_Children)
		{
			if (child)
				child->m_Parent = this;
		}
		m_GetAABB = std::move(move.m_GetAABB);
		m_Section = move.m_Section;
		m_Content = std::move(move.m_Content);

		m_Center = std::move(move.m_Center);
		m_Radius = move.m_Radius;
	}

	Octree_Impl &operator=(const Octree_Impl &copy)
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (copy.m_Children[i])
			{
				m_Children[i] =
				    std::make_unique<Octree_Impl>(*copy.m_Children[i]);
				m_Children[i]->m_Parent = this;
			}
		}
		m_GetAABB = copy.m_GetAABB;
		m_Section = copy.m_Section;
		m_Content = copy.m_Content;

		m_Center = copy.m_Center;
		m_Radius = copy.m_Radius;

		return *this;
	}
	Octree_Impl &operator=(Octree_Impl &&move)
	{
		m_Parent = move.m_Parent;
		m_TopLevel = move.m_TopLevel;
		m_Children = std::move(move.m_Children);
		for (auto &child : m_Children)
		{
			if (child)
				child->m_Parent = this;
		}
		m_GetAABB = std::move(move.m_GetAABB);
		m_Section = move.m_Section;
		m_Content = std::move(move.m_Content);

		m_Center = std::move(move.m_Center);
		m_Radius = move.m_Radius;

		return *this;
	}

	void Add(size_t id);
	bool Update(size_t id);
	void Remove(size_t id);

	std::vector<size_t> GetColliding(size_t id);
};
} // namespace Detail

class Octree
{
	friend class Detail::Octree_Impl;

	void OutOfBounds(size_t id);

	std::unique_ptr<Detail::Octree_Impl> m_Root;
	std::function<std::array<glm::dvec3, 2>(size_t)> m_GetAABB;

	public:
	Octree(decltype(m_GetAABB) GetAABB) : m_GetAABB(GetAABB)
	{
		m_Root = std::make_unique<Detail::Octree_Impl>(GetAABB);
	}

	Octree(const Octree &copy)
	{
		m_GetAABB = copy.m_GetAABB;
		m_Root = std::make_unique<Detail::Octree_Impl>(*copy.m_Root);
		m_Root->m_TopLevel = this;
	}
	Octree(Octree &&move)
	{
		m_GetAABB = std::move(move.m_GetAABB);
		m_Root = std::move(move.m_Root);
		m_Root->m_TopLevel = this;
	}
	Octree &operator=(const Octree &copy)
	{

		m_GetAABB = copy.m_GetAABB;
		m_Root = std::make_unique<Detail::Octree_Impl>(*copy.m_Root);
		m_Root->m_TopLevel = this;

		return *this;
	}
	Octree &operator=(Octree &&move)
	{
		m_GetAABB = std::move(move.m_GetAABB);
		m_Root = std::move(move.m_Root);
		m_Root->m_TopLevel = this;

		return *this;
	}

	void Add(size_t id);
	void Update(size_t id);
	void Remove(size_t id);
};