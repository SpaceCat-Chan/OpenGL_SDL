#pragma once

#include <bitset>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include <glm/ext.hpp>

#include "Camera/Camera.hpp"
#include "Mesh/Mesh.hpp"
#include "Octree/Octree.hpp"
#include "TexturedMesh/TexturedMesh.hpp"

#include "Components/Children/Children.hpp"
#include "Components/LightInfo/LightInfo.hpp"
#include "Components/Meshes/Meshes.hpp"
#include "Components/Transform/Transform.hpp"
#include "Components/UserInput/UserInput.hpp"

#include "Systems/AutoPosition/AutoPosition.hpp"
#include "Systems/Render/Render.hpp"
#include "Systems/UserInput/UserInput.hpp"

#include "Common.hpp"

#include "TransformedMinMax.hpp"

/**
 * \brief a class that contains information about an error
 */
struct Error
{
	Error() = default;
	enum class Type
	{
		None
	};
	Error(Type) {}
};

/**
 * \brief a class that handles the entire gameworld
 */
struct World
{
	bool Quit = false;

	enum Components
	{
		Position,
		Mesh,
		Light,
		Transform,
		Children
	};

	static constexpr size_t ComponentAmount = 5;

	private:
	std::vector<std::optional<glm::dvec3>> PositionComponents;
	std::vector<std::optional<Meshes>> MeshComponents;
	std::vector<std::optional<LightInfo>> LightComponents;
	std::vector<std::optional<::Transform>> TransformComponents;
	std::vector<std::optional<::Children>> ChildrenComponents;

	public:
	class EntityReferanceWrapper
	{
		World *m_World;
		std::size_t m_Index;
		EntityReferanceWrapper(World &GameWorld, std::size_t Index)
		{
			m_World = &GameWorld;
			m_Index = Index;
		}

		friend class ::World;

		public:
		EntityReferanceWrapper() = delete;

		EntityReferanceWrapper(const EntityReferanceWrapper &) = delete;
		EntityReferanceWrapper(EntityReferanceWrapper &&) = default;
		EntityReferanceWrapper &
		operator=(const EntityReferanceWrapper &) = delete;
		EntityReferanceWrapper &operator=(EntityReferanceWrapper &&) = delete;

		std::optional<glm::dvec3> &Position() { return m_World->PositionComponents[m_Index]; }
		std::optional<Meshes> &Mesh() { return m_World->MeshComponents[m_Index]; }
		std::optional<LightInfo> &Light() { return m_World->LightComponents[m_Index]; }
		std::optional<::Transform> &Transform() { return m_World->TransformComponents[m_Index]; }
		std::optional<::Children> &Children() { return m_World->ChildrenComponents[m_Index]; }

		void reset()
		{
			Position() = std::nullopt;
			Mesh() = std::nullopt;
			Light() = std::nullopt;
			Transform() = std::nullopt;
			Children() = std::nullopt;
		}
	};

	std::size_t size() { return PositionComponents.size(); }

	inline EntityReferanceWrapper operator[](std::size_t Index)
	{
		return EntityReferanceWrapper{*this, Index};
	}

	std::list<size_t> UpdatedEntities;

	Octree CollisionOctree{[this](size_t id) -> std::array<glm::dvec3, 2> {
		glm::dmat4x4 MeshTransform;
		auto &GameWorld = *this;
		if (GameWorld[id].Children())
		{
			MeshTransform =
			    GameWorld[id].Children()->CalculateFullTransform(GameWorld, id);
		}
		else if (GameWorld[id].Transform())
		{
			MeshTransform = GameWorld[id].Transform()->CalculateFull();
		}
		else
		{
			MeshTransform = glm::dmat4x4(1);
		}
		auto [Min, Max] =
		    CalculateTransformedMinMax(*GameWorld[id].Mesh(), MeshTransform);
		return {Min, Max};
	}};

	std::vector<std::function<Error(World &, DSeconds)>> Systems{
	    UserInputSystem,
	    AutoPositionSystem,
	    RenderSystem};

	Shader ShaderProgram;
	Camera View;

	std::queue<size_t> UnusedIDs;

	/**
	 * \brief Creates and return a new Entity ID
	 */
	size_t NewEntity()
	{
		if (!UnusedIDs.empty())
		{
			size_t Result = UnusedIDs.front();
			UnusedIDs.pop();
			return Result;
		}
		else
		{
			PositionComponents.push_back(std::nullopt);
			MeshComponents.push_back(std::nullopt);
			LightComponents.push_back(std::nullopt);
			TransformComponents.push_back(std::nullopt);
			ChildrenComponents.push_back(std::nullopt);
			return PositionComponents.size() - 1;
		}
	}

	void DeleteEntity(size_t ID)
	{
		operator[](ID).reset();
		UnusedIDs.push(ID);
	}

	size_t CloneEntity(size_t ID)
	{
		size_t Clone = NewEntity();
		std::cout << "Old ID: " << ID << " New ID: " << Clone << '\n';
		PositionComponents.at(Clone) = PositionComponents.at(ID);
		MeshComponents.at(Clone) = MeshComponents.at(ID);
		LightComponents.at(Clone) = LightComponents.at(ID);
		TransformComponents.at(Clone) = TransformComponents.at(ID);
		ChildrenComponents.at(Clone) = ChildrenComponents.at(ID);
		return Clone;
	}
};

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Position, void>
ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World[ID].Position() = glm::dvec3(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Mesh, void>
ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World[ID].Mesh() = Meshes(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Light, void>
ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World[ID].Light() = LightInfo(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Transform, void>
ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World[ID].Transform() = Transform(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Children, void>
ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World[ID].Children() = Children(std::forward<Args>(args)...);
}