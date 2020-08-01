#pragma once

#include <optional>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "Mesh/Mesh.hpp"
#include "Octree/Octree.hpp"
#include "TexturedMesh/TexturedMesh.hpp"

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
	static std::vector<std::optional<Octree>> StaticOctrees;
	/**
	 * \brief all the known TexturedMeshes
	 *
	 * if the order changes then existing saves/mapfiles may be broken
	 */
	static std::vector<TexturedMesh> TexturedMeshes;
	static std::vector<std::optional<Octree>> TexturedOctrees;

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

	static std::array<glm::dvec3,3> Custom;

	static void SetupOctree(MeshType Type, size_t index)
	{
		StaticOctrees.resize(StaticMeshes.size());
		TexturedOctrees.resize(TexturedMeshes.size());

		if (Type == MeshType::Static)
		{
			if (!StaticOctrees[index])
			{
				StaticOctrees[index] = Octree{
				    [index](size_t id) -> std::array<glm::dvec3, 2> {
					    std::array<glm::dvec3, 3> Triangle = {
					        StaticMeshes[index].Triangles()[id * 3],
					        StaticMeshes[index].Triangles()[id * 3 + 1],
					        StaticMeshes[index].Triangles()[id * 3 + 2]};
						if(id == size_t(-1))
						{
							Triangle = Meshes::Custom;
						}
					    glm::dvec3 Min = {
					        std::min(
					            std::min(Triangle[0].x, Triangle[1].x),
					            Triangle[2].x),
					        std::min(
					            std::min(Triangle[0].y, Triangle[1].y),
					            Triangle[2].y),
					        std::min(
					            std::min(Triangle[0].z, Triangle[1].z),
					            Triangle[2].z)};
					    glm::dvec3 Max = {
					        std::max(
					            std::max(Triangle[0].x, Triangle[1].x),
					            Triangle[2].x),
					        std::max(
					            std::max(Triangle[0].y, Triangle[1].y),
					            Triangle[2].y),
					        std::max(
					            std::max(Triangle[0].z, Triangle[1].z),
					            Triangle[2].z)};
					    return {Min, Max};
				    },
				    1};
				for (size_t i = 0;
				     i < StaticMeshes[index].Triangles().size() / 3;
				     i++)
				{
					StaticOctrees[index]->Add(i);
				}
			}
		}
		else
		{
			if (!TexturedOctrees[index])
			{
				TexturedOctrees[index] = Octree{
				    [index](size_t id) -> std::array<glm::dvec3, 2> {
					    std::array<glm::dvec3, 3> Triangle = {
					        TexturedMeshes[index].Triangles()[id * 3],
					        TexturedMeshes[index].Triangles()[id * 3 + 1],
					        TexturedMeshes[index].Triangles()[id * 3 + 2]};
					    glm::dvec3 Min = {
					        std::min(
					            std::min(Triangle[0].x, Triangle[1].x),
					            Triangle[2].x),
					        std::min(
					            std::min(Triangle[0].y, Triangle[1].y),
					            Triangle[2].y),
					        std::min(
					            std::min(Triangle[0].z, Triangle[1].z),
					            Triangle[2].z)};
					    glm::dvec3 Max = {
					        std::max(
					            std::max(Triangle[0].x, Triangle[1].x),
					            Triangle[2].x),
					        std::max(
					            std::max(Triangle[0].y, Triangle[1].y),
					            Triangle[2].y),
					        std::max(
					            std::max(Triangle[0].z, Triangle[1].z),
					            Triangle[2].z)};
					    return {Min, Max};
				    },
				    1};
				for (size_t i = 0;
				     i < TexturedMeshes[index].Triangles().size() / 3;
				     i++)
				{
					TexturedOctrees[index]->Add(i);
				}
			}
		}
	}
};