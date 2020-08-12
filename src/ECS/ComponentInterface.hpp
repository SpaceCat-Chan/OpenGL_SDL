#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

struct World;

glm::dmat4
CalcTransform(const World &GameWorld, size_t id, bool UseBackup = false);
glm::dmat4 CalcCollisionTransform(
    const World &GameWorld,
    size_t id,
    bool UseBackup = false);
glm::dmat4 CalcCollisionModelTransform(const World &GameWorld, size_t id);