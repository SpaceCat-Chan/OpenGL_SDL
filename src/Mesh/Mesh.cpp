#include "Mesh.hpp"
#include "OBJ_Loader.h"

void Mesh::LoadMesh(std::string Filename)
{
	objl::Loader OBJ;

	OBJ.LoadFile(Filename);

	glBindVertexArray(m_VertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer[0]);

	std::vector<GLfloat> m_Positions;
	std::vector<GLfloat> m_UVCoords;
	for (size_t i = 0; i < OBJ.LoadedMeshes[0].Vertices.size(); i++)
	{
		m_Positions.push_back(OBJ.LoadedMeshes[0].Vertices[i].Position.X);
		m_Positions.push_back(OBJ.LoadedMeshes[0].Vertices[i].Position.Y);
		m_Positions.push_back(OBJ.LoadedMeshes[0].Vertices[i].Position.Z);

		m_UVCoords.push_back(OBJ.LoadedMeshes[0].Vertices[i].TextureCoordinate.X);
		m_UVCoords.push_back(OBJ.LoadedMeshes[0].Vertices[i].TextureCoordinate.Y);
	}

	glBufferData(GL_ARRAY_BUFFER, m_Positions.size() * sizeof(GLfloat), m_Positions.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, m_UVCoords.size() * sizeof(GLfloat), m_UVCoords.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, OBJ.LoadedMeshes[0].Indices.size() * sizeof(GLuint), OBJ.LoadedMeshes[0].Indices.data(), GL_STATIC_DRAW);
	m_IndexAmount = OBJ.LoadedMeshes[0].Indices.size();
}