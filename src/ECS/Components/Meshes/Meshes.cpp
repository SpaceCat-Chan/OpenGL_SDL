#include "Meshes.hpp"

std::vector<Mesh> Meshes::StaticMeshes;
std::vector<TexturedMesh> Meshes::TexturedMeshes;
std::vector<std::optional<Octree>> Meshes::StaticOctrees;
std::vector<std::optional<Octree>> Meshes::TexturedOctrees;
std::array<glm::dvec3,3> Meshes::Custom{};