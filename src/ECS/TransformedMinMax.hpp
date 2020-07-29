#pragma once

#include <tuple>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>

#include "Mesh/Mesh.hpp"
#include "TexturedMesh/TexturedMesh.hpp"
#include "Components/Meshes/Meshes.hpp"

/**
 * \brief takes two vectors and scales them up by Amount relative to the point
 * between them
 *
 * \param Min one point to be scaled
 * \param Max one point to be scaled
 *
 * \param Amount the amount to scale Min and Max by
 *
 * \return the new Min and Max
 */
std::tuple<glm::dvec3, glm::dvec3>
ExpandArea(glm::dvec3 Min, glm::dvec3 Max, double Amount = std::sqrt(2.0));

template <class MeshType>
std::enable_if_t<
    std::is_same<MeshType, Mesh>::value ||
        std::is_same<MeshType, TexturedMesh>::value,
    std::tuple<glm::dvec3, glm::dvec3>>
CalculateTransformedMinMax(MeshType &Mesh, glm::dmat4x4 Transform)
{
	std::array<glm::vec3, 6> VertexExtremes;

	VertexExtremes[0] =
	    Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegX), 1);
	VertexExtremes[1] =
	    Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegY), 1);
	VertexExtremes[2] =
	    Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::NegZ), 1);
	VertexExtremes[4] =
	    Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosY), 1);
	VertexExtremes[5] =
	    Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosZ), 1);
	VertexExtremes[3] =
	    Transform * glm::vec4(Mesh.GetMostExtremeVertex(Mesh::Side::PosX), 1);

	glm::dvec3 ResultMin = {
	    VertexExtremes[0].x,
	    VertexExtremes[1].y,
	    VertexExtremes[2].z};

	glm::dvec3 ResultMax = {
	    VertexExtremes[3].x,
	    VertexExtremes[4].y,
	    VertexExtremes[5].z};

	// rotation may change which side the extremes belong to
	for (auto &Vertex : VertexExtremes)
	{
		if (Vertex.x < ResultMin.x)
		{
			ResultMin.x = Vertex.x;
		}
		if (Vertex.y < ResultMin.y)
		{
			ResultMin.y = Vertex.y;
		}
		if (Vertex.z < ResultMin.z)
		{
			ResultMin.z = Vertex.z;
		}

		if (Vertex.x > ResultMax.x)
		{
			ResultMax.x = Vertex.x;
		}
		if (Vertex.y > ResultMax.y)
		{
			ResultMax.y = Vertex.y;
		}
		if (Vertex.z > ResultMax.z)
		{
			ResultMax.z = Vertex.z;
		}
	}

	return ExpandArea(ResultMin, ResultMax);
}

template <class MeshType>
std::enable_if_t<
    std::is_same<MeshType, Meshes>::value,
    std::tuple<glm::dvec3, glm::dvec3>>
CalculateTransformedMinMax(MeshType Mesh, glm::dmat4x4 Transform)
{
	if (Mesh.Type == Meshes::MeshType::Static)
	{
		return CalculateTransformedMinMax(
		    Meshes::StaticMeshes[Mesh.MeshIndex],
		    Transform);
	}
	else
	{
		return CalculateTransformedMinMax(
		    Meshes::TexturedMeshes[Mesh.MeshIndex],
		    Transform);
	}
}