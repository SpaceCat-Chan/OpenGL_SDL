#pragma once

#include <optional>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "ECS/Components/Transform/Transform.hpp"

//no it does not backup to a file
//it makes a backup of the last tick
//used in rendering interp and collision rollback
struct BasicBackup {
	std::optional<glm::dvec3> Position;
	std::optional<Transform> Transform_;
};