#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

struct Force
{
	glm::dvec3 ApplyPosition;
	glm::dvec3 Force;
};