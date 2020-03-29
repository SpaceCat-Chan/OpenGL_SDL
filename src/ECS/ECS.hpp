#pragma once

#include <vector>
#include <bitset>
#include <functional>
#include <queue>
#include <memory>

#include <glm/ext.hpp>

#include "Mesh/Mesh.hpp"
#include "TexturedMesh/TexturedMesh.hpp"
#include "Camera/Camera.hpp"

#include "Common.hpp"

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
 * \brief a class for keeping track of a mesh in a serializable way
 */
struct Meshes
{
	/**
	 * \brief all the known meshes
	 * 
	 * if the order changes then existing saves/mapfiles may be broken
	 */
	static std::vector<Mesh> StaticMeshes;
	/**
	 * \brief all the known TexturedMeshes
	 * 
	 * if the order changes then existing saves/mapfiles may be broken
	 */
	static std::vector<TexturedMesh> TexturedMeshes;

	enum class MeshType
	{
		Static,
		Textured
	};

	/**
	 * \brief the type of mesh being stored
	 */
	MeshType Type = MeshType::Static;
	/**
	 * \brief the index in either StaticMeshes or TexturedMeshes
	 * 
	 * which array is defined by Meshes::Type
	 */
	size_t MeshIndex = 0;
	/**
	 * \brief if the mesh should have ligting applied to it
	 */
	bool AffectedByLights = true;

	Meshes() = default;
	Meshes(const Meshes &) = default;
	Meshes(Meshes &&) = default;
	Meshes(MeshType _Type, size_t _Index, bool _AffectedByLights = true)
	{
		Type = _Type;
		MeshIndex = _Index;
		AffectedByLights = _AffectedByLights;
	}
	~Meshes() = default;

	constexpr Meshes &operator=(const Meshes &) = default;
	constexpr Meshes &operator=(Meshes &&) = default;
};

struct LightInfo
{
	enum class Type
	{
		Point,
		Direction
	};
	/**
	 * \brief the color of the light
	 */
	glm::dvec3 Color = {1, 1, 1};

	/**
	 * \brief wether the light is a point light or a directional light
	 * 
	 * exmaples:
	 * point light - a light bulb
	 * directional light - the sun
	 */
	Type LightType = Type::Point;
	/**
	 * \brief position of the light
	 * 
	 * ignored for directional lights
	 */
	glm::dvec3 Position = {0, 0, 0};
	/**
	 * \brief the direction the light is pointing in
	 * 
	 * ignored for point lights with default cutoff angle
	 */
	glm::dvec3 Direction = {0, 0, 0};

	/**
	 * \brief the angle where the light gets cut off
	 * 
	 * ignored for directional lights
	 */
	double CutoffAngle = glm::pi<double>() * 2;
	/**
	 * \brief the max distance the light can affect objects
	 * 
	 * ignored for directional lights
	 */
	double CutoffDistance = 100;

	/**
	 * \name attenuation
	 * 
	 * theese variables are needed for calculating the attenuation of the light, given by this formula:
	 * 
	 * 1.0 / (Constant + Linear * Distance + Quadratic * pow(distance, 2))
	 * 
	 * the standard is tuned for a max distance of 100
	 * 
	 * ignored by directional lights  
	 */
	///@{
	double Constant = 1;
	double Linear = 0.045;
	double Quadratic = 0.0075;
	///@}

	LightInfo() = default;
	LightInfo(const LightInfo &) = default;
	LightInfo(LightInfo &&) = default;
	LightInfo(Type _LightType, double _CutoffDistance, double _CutoffAngle = glm::pi<double>()*2, double _Constant = 1, double _Linear = 0.045, double _Quadratic = 0.0075)
	{
		LightType = _LightType;
		CutoffAngle = _CutoffAngle;
		CutoffDistance = _CutoffDistance;
		Constant = _Constant;
		Linear = _Linear;
		Quadratic = _Quadratic;
	}

	constexpr LightInfo &operator=(const LightInfo &) = default;
	constexpr LightInfo &operator=(LightInfo &&) = default;
};

/**
 * \brief the component containing information about how to transform from Model space to View space
 */
struct Transform
{
	enum class Space
	{
		Tangent,
		Model,
		World,
		View,
		Clip
	};

	/**
	 * Type::Mix should be used for matrixes with unknown transformations
	 * 
	 * 
	 * 
	 * about Type::AutoPosition:
	 * 
	 * the first Matrix in Transform::Tranformations that is of type Type::AutoPosition will be set to a translation matrix to translate everything by the amount specified in the position compoent
	 * 
	 * all other matrixes with type Type::AutoPosition will be set to the identity matrix
	 * 
	 * this is only true if:
	 * 
	 * 1. there is a position component active for this entity
	 * 2. PositionComponentSpace is set to Space::View
	 * 3. ::AutoPositionSystem is active
	 * 
	 * and it will only be updated after ::AutoPositionSystem has been run
	 */
	enum class Type
	{
		Translate,
		Rotate,
		Scale,
		Mix,
		AutoPosition
	};

	/**
	 * \brief the space that the PositionComponent is in
	 * 
	 * it is not expected for the value to be anything other than Space::Model or Space::View, but is can be other things too
	 */
	Space PositionComponentSpace = Space::View;
	/**
	 * \brief the space that positions of other components are in
	 * 
	 * it is not expected for the value to be anything other than Space::Model or Space::View, but is can be other things too
	 */
	Space PositionSpace = Space::Model;

	/**
	 * \brief a list of transformations
	 * 
	 * Transformations[i].first contains the type of tranformation that matrix does
	 * 
	 * Transformations[i].second contains the actual matrix
	 */
	std::vector<std::pair<Type, glm::mat4x4>> Tranformations;

	/**
	 * \brief calculates the full transformation matrix
	 * 
	 * \return the full transformation matrix
	 */
	glm::mat4x4 CalculateFull()
	{
		glm::mat4x4 Result(1);
		for(auto &Matrix : Tranformations)
		{
			Result = Matrix.second * Result;
		}
		return Result;
	}

	/**
	 * \brief checks if there are any rotation transformations
	 * 
	 * \return if there are any rotation transformations
	 */
	bool ContainsRotations()
	{
		for(auto &Matrix : Tranformations)
		{
			if(Matrix.first == Type::Rotate || Matrix.first == Type::Mix)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * \brief applies the entire transformation to a vector
	 * 
	 * \param V the vector to transform
	 * \param UseRotations wether or not to ignore Matrixes with type Type::Rotate and Type::Mix
	 * 
	 * \return the transformed vector
	 */
	glm::dvec4 operator()(glm::dvec4 V, bool UseRotations=true)
	{
		if(UseRotations)
		{
			return CalculateFull() * V;
		}
		else
		{
			for(auto &Matrix : Tranformations)
			{
				if(Matrix.first == Type::Rotate || Matrix.first == Type::Mix)
				{
					continue;
				}
				V = Matrix.second * V;
			}
			return V;
		}
	}
};

struct World;

/**
 * \brief the system responsible for updating Matrixes of type Transform::Type::AutoPosition
 */
Error AutoPositionSystem(World &GameWorld, DSeconds dt);
Error RenderSystem(World &GameWorld, DSeconds dt);

/**
 * \brief a class that handles the entire gameworld
 */
struct World
{

	enum Components
	{
		Position,
		Mesh,
		Light,
		Transform
	};

	static constexpr size_t ComponentAmount = 4;

	std::vector<std::shared_ptr<glm::dvec3>> PositionComponents;
	std::vector<std::shared_ptr<Meshes>> MeshComponents;
	std::vector<std::shared_ptr<LightInfo>> LightComponents;
	std::vector<std::shared_ptr<::Transform>> TransformComponents;

	std::vector<std::function<Error(World &, DSeconds)>> Systems{AutoPositionSystem, RenderSystem};

	Shader ShaderProgram;
	Camera View;


	std::queue<size_t> UnusedIDs;

	/**
	 * \brief Creates and return a new Entity ID
	 */
	size_t NewEntity()
	{
		if(UnusedIDs.empty() == false)
		{
			size_t Result = UnusedIDs.front();
			UnusedIDs.pop();
			return Result;
		}
		else
		{
			PositionComponents.emplace_back(nullptr);
			MeshComponents.emplace_back(nullptr);
			LightComponents.emplace_back(nullptr);
			TransformComponents.emplace_back(nullptr);
			return PositionComponents.size() - 1;
		}
	}

	void DeleteEntity(size_t ID)
	{
		PositionComponents[ID] = nullptr;
		MeshComponents[ID] = nullptr;
		LightComponents[ID] = nullptr;
		TransformComponents[ID] = nullptr;
		UnusedIDs.push(ID);
	}
};

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Position, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.PositionComponents[ID] = std::make_shared<glm::dvec3>(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Mesh, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.MeshComponents[ID] = std::make_shared<Meshes>(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Light, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.LightComponents[ID] = std::make_shared<LightInfo>(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Transform, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.TransformComponents[ID] = std::make_shared<Transform>(std::forward<Args>(args)...);
}
