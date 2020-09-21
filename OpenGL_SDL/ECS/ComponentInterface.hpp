#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

struct World;
struct Meshes;
struct Shader;

glm::dmat4
CalcTransform(const World &GameWorld, size_t id, bool UseBackup = false);
glm::dmat4 CalcCollisionTransform(
    const World &GameWorld,
    size_t id,
    bool UseBackup = false);
glm::dmat4 CalcCollisionModelTransform(const World &GameWorld, size_t id);

Meshes CollisionMesh(const World &GameWorld, size_t id);

Shader &EntityShader(World &GameWorld, size_t id, bool ForceMainShader);