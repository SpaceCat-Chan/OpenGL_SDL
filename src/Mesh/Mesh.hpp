#pragma once

#include <vector>
#include <string>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

#include "SDL-Helper-Libraries/sfstream/sfstream.hpp"

#include "tiny_obj_loader.h"


/**
 * \brief a struct responsible for handeling information related to Materials
 */
struct Material {
	std::string Name;

	glm::dvec3 Ambient = {0.2, 0.2, 0.2};
	glm::dvec3 Diffuse = {0.8, 0.8, 0.8};
	glm::dvec3 Specular = {1.0, 1.0, 1.0};
	double SpecularWeight = 0;

	double Alpha = 1;

	uint8_t IlluminationMode = 1;
};


/**
 * \brief Class for loading and handeling meshes
 * 
 * meshes are loaded from obj files
 */
class Mesh
{
	GLuint m_VertexArray = 0;
	GLuint m_VertexBuffer[3];
	std::vector<GLuint> m_IndexBuffer;
	std::vector<size_t> m_IndexAmount;



public:
	std::vector<Material> Materials;
	Mesh()
	{
		glGenBuffers(3, m_VertexBuffer);
		glGenVertexArrays(1, &m_VertexArray);
	}
	~Mesh()
	{
		glDeleteBuffers(3, m_VertexBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);

		for(size_t i=0; i<m_IndexBuffer.size(); i++) {
			glDeleteBuffers(1, &m_IndexBuffer[i]);
		}
	}

	void LoadMesh(std::string Filename);

	void Bind(size_t MeshIndex)
	{
		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer[MeshIndex]);
	}

	size_t GetIndexCount(size_t MeshIndex) { return m_IndexAmount[MeshIndex]; }
};