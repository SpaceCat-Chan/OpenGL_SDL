#pragma once

#include <vector>
#include <bitset>
#include <functional>

#include <glm/ext.hpp>

#include "Mesh/Mesh.hpp"
#include "TexturedMesh/TexturedMesh.hpp"
#include "Camera/Camera.hpp"

#include "Common.hpp"

struct Error
{
	enum class ErrorType
	{
		None,
		Warning,
		NonFatal,
		Fatal
	};

	ErrorType Severity;
	std::string Message;

	Error(ErrorType _Severity, std::string _Message = "") : Severity(_Severity), Message(_Message)
	{
	}
};

struct Meshes
{
	static std::vector<Mesh> StaticMeshes;
	static std::vector<TexturedMesh> TexturedMeshes;

	enum class MeshType
	{
		None,
		Static,
		Textured
	};

	MeshType Type = MeshType::None;
	size_t MeshIndex = 0;
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

	glm::dvec3 Color = {1, 1, 1};

	Type LightType = Type::Point;
	glm::dvec3 Position = {0, 0, 0};

	double CutoffAngle = glm::pi<double>();
	double CutoffDistance = 100;

	double Constant = 1;
	double Linear = 0.045;
	double Quadratic = 0.0075;

	LightInfo() = default;
	LightInfo(const LightInfo&) = default;
	LightInfo(LightInfo&&) = default;
	LightInfo(Type _LightType, double _CutoffAngle, double _CutoffDistance, double _Constant=1, double _Linear=0.045, double _Quadratic=0.0075)
	{
		LightType = _LightType;
		CutoffAngle = _CutoffAngle;
		CutoffDistance = _CutoffDistance;
		Constant = _Constant;
		Linear = _Linear;
		Quadratic = _Quadratic;
	}
};

struct World;

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
		Light
	};

	static constexpr size_t ComponentAmount = 3;

	std::vector<std::bitset<ComponentAmount>> ComponentMask;

	std::vector<glm::dvec3> PositionComponents;
	std::vector<Meshes> MeshComponents;
	std::vector<LightInfo> LightComponents;

	std::vector<std::function<Error(World &, DSeconds)>> Systems{RenderSystem};

	Shader ShaderProgram;
	Camera View;

	size_t NewEntity()
	{
		ComponentMask.push_back(std::bitset<ComponentAmount>());

		PositionComponents.push_back({0, 0, 0});
		MeshComponents.push_back({});
		return PositionComponents.size() - 1;
	}
};




template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Position, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.ComponentMask[ID][World::Position] = true;
	World.PositionComponents[ID] = glm::dvec3(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Mesh, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.ComponentMask[ID][World::Mesh] = true;
	World.MeshComponents[ID] = Meshes(std::forward<Args>(args)...);
}

template <size_t Component, typename... Args>
std::enable_if_t<Component == World::Light, void> ActivateComponent(size_t ID, World &World, Args &&... args)
{
	World.ComponentMask[ID][World::Light] = true;
	World.MeshComponents[ID] = Meshes(std::forward<Args>(args)...);
}
