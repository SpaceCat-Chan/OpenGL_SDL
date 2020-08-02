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

#include "Components/BasicBackup/BasicBackup.hpp"
#include "Components/Children/Children.hpp"
#include "Components/LightInfo/LightInfo.hpp"
#include "Components/Meshes/Meshes.hpp"
#include "Components/Transform/Transform.hpp"
#include "Components/UserInput/UserInput.hpp"
#include "Components/Collision/Collision.hpp"

#include "Systems/AutoPosition/AutoPosition.hpp"
#include "Systems/BasicBackup/BasicBackup.hpp"
#include "Systems/Collision/Collision.hpp"
#include "Systems/Render/Render.hpp"
#include "Systems/UserInput/UserInput.hpp"

#include "Common.hpp"

#include "TransformedMinMax.hpp"

/**
 * \brief a class that contains information about an error
 */
struct Error
{
	enum class Type
	{
		None,
		Warning,
		NonFatal,
		Fatal
	};

	/**
	 * \brief how severe the error is
	 */
	Type Severity;
	/**
	 * \brief a message explaining the error
	 */
	std::string Message;

	Error(Type _Severity, std::string _Message = "") : Severity(_Severity), Message(_Message)
	{
	}
};

/**
 * \brief a class that handles the entire gameworld
 */
struct World
{
	bool Quit = false;



	private:
	std::vector<std::optional<glm::dvec3>> PositionComponents;
	std::vector<std::optional<Meshes>> MeshComponents;
	std::vector<std::optional<LightInfo>> LightComponents;
	std::vector<std::optional<Transform>> TransformComponents;
	std::vector<std::optional<Children>> ChildrenComponents;
	std::vector<std::optional<BasicBackup>> BackupComponents;
	std::vector<std::optional<Collision>> CollisionComponents;

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

		friend struct ::World;

		public:
		EntityReferanceWrapper() = delete;

		EntityReferanceWrapper(const EntityReferanceWrapper &) = delete;
		EntityReferanceWrapper(EntityReferanceWrapper &&) = default;
		EntityReferanceWrapper &
		operator=(const EntityReferanceWrapper &) = delete;
		EntityReferanceWrapper &operator=(EntityReferanceWrapper &&) = delete;

		std::optional<glm::dvec3> &Position()
		{
			return m_World->PositionComponents[m_Index];
		}
		std::optional<Meshes> &Mesh()
		{
			return m_World->MeshComponents[m_Index];
		}
		std::optional<LightInfo> &Light()
		{
			return m_World->LightComponents[m_Index];
		}
		std::optional<Transform> &Transform()
		{
			return m_World->TransformComponents[m_Index];
		}
		std::optional<Children> &Children()
		{
			return m_World->ChildrenComponents[m_Index];
		}
		std::optional<BasicBackup> &BasicBackup()
		{
			return m_World->BackupComponents[m_Index];
		}
		std::optional<Collision> &Collision()
		{
			return m_World->CollisionComponents[m_Index];
		}

		void reset()
		{
			Position() = std::nullopt;
			Mesh() = std::nullopt;
			Light() = std::nullopt;
			Transform() = std::nullopt;
			Children() = std::nullopt;
			BasicBackup() = std::nullopt;
			Collision() = std::nullopt;
		}
	};

	std::size_t size() { return PositionComponents.size(); }

	inline EntityReferanceWrapper operator[](std::size_t Index)
	{
		return EntityReferanceWrapper{*this, Index};
	}

	std::list<size_t> UpdatedEntities;

	Octree CollisionOctree{[this](size_t id) -> std::array<glm::dvec3, 2> {
		auto &GameWorld = *this;
		if (GameWorld[id].Mesh())
		{
			glm::dmat4x4 MeshTransform;
			if (GameWorld[id].Children())
			{
				MeshTransform =
				    GameWorld[id].Children()->CalculateFullTransform(
				        GameWorld,
				        id);
			}
			else if (GameWorld[id].Transform())
			{
				MeshTransform = GameWorld[id].Transform()->CalculateFull();
			}
			else
			{
				MeshTransform = glm::dmat4x4(1);
			}
			auto [Min, Max] = CalculateTransformedMinMax(
			    *GameWorld[id].Mesh(),
			    MeshTransform);
			return {Min, Max};
		}
		else
		{
			return {glm::dvec3{-1.0, -1.0, -1.0}, glm::dvec3{1.0, 1.0, 1.0}};
		}
	}};

	std::vector<
	    std::vector<std::pair<bool, std::function<Error(World &, DSeconds)>>>>
	    UpdateSystems{
	        {{true, BasicBackupSystem}},
	        {{true, UserInputSystem}},
	        {{true, AutoPositionSystem}},
	        {{true, UpdateOctree}},
			{{true, HandleCollisions}}};

	std::vector<std::function<Error(World &, DSeconds)>> RenderSystems{
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
			BackupComponents.push_back(BasicBackup{});
			CollisionComponents.push_back(std::nullopt);
			CollisionOctree.Add(PositionComponents.size() - 1);
			UpdatedEntities.push_back(PositionComponents.size() - 1);
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
		BackupComponents.at(Clone) = BackupComponents.at(ID);
		return Clone;
	}
};