#pragma once

#include <vector>
#include <bitset>
#include <functional>

#include <glm/ext.hpp>

#include "Mesh/Mesh.hpp"
#include "TexturedMesh/TexturedMesh.hpp"
#include "Camera/Camera.hpp"

struct Error {
	enum class ErrorType {
		None, Warning, NonFatal, Fatal
	};

	ErrorType Severity;
	std::string Message;

	Error(ErrorType _Severity, std::string _Message="") : Severity(_Severity), Message(_Message)
	{}
};

struct Meshes {
	static std::vector<Mesh> StaticMeshes;
	static std::vector<TexturedMesh> TexturedMeshes;

	enum class MeshType {
		None,
		Static,
		Textured
	};

	MeshType Type=MeshType::None;
	size_t MeshIndex=0;
};

Error RenderSystem(World& GameWorld);

/**
 * \brief a class that handles the entire gameworld
 */
struct World {
	
	enum Components {
		Position, Mesh
	};

	static constexpr size_t ComponentAmount=2;

	std::vector<std::bitset<ComponentAmount>> ComponentMask;

	std::vector<glm::dvec3> PositionComponents;
	std::vector<Meshes> MeshComponents;

	std::vector<std::function<Error(World&)>> Systems{RenderSystem};

	Shader ShaderProgram;
	Camera View;

	size_t NewEntity() {
		ComponentMask.push_back(std::bitset<ComponentAmount>());

		PositionComponents.push_back({0, 0, 0});
		MeshComponents.push_back({});
		return PositionComponents.size() - 1;
	}
};

template<size_t Component, typename... Args>
std::enable_if<Component == World::Components::Position, void> AvtivateComponent(size_t ID, World& World, Args&&... args) {
	World.ComponentMask[Component] = true;
	World.PositionComponents[ID] = glm::dvec3(std::forward<Args>(args)...)
}

template<size_t Component, typename... Args>
std::enable_if<Component == World::Components::Mesh, void> AvtivateComponent(size_t ID, World& World, Args&&... args) {
	World.ComponentMask[Component] = true;
	World.MeshComponents[ID] = Meshes(std::forward<Args>(args)...)
}
