#pragma once

#include <vector>

#include "Mesh/Mesh.hpp"
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