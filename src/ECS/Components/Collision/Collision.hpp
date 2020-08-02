#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "ECS/Components/Meshes/Meshes.hpp"


class
{
	public:
	std::optional<Meshes> CollisionMesh;
	glm::dmat4 CollisionMeshToModelSpace{0};

	glm::vec3 CenterOfMass; //Usused for now
};