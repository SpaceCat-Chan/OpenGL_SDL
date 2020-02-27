#pragma once

#include <vector>
#include <string>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

/**
 * \brief Class for loading and handeling meshes
 * 
 * meshes are loaded from obj files
 */
class Mesh
{
	unsigned int m_VertexArray = 0;
	unsigned int m_VertexBuffer[3];
	unsigned int m_IndexBuffer = 0;
	size_t m_IndexAmount = 0;

public:
	Mesh()
	{
		glGenBuffers(3, m_VertexBuffer);
		glGenBuffers(1, &m_IndexBuffer);
		glGenVertexArrays(1, &m_VertexArray);
	}
	~Mesh()
	{
		glDeleteBuffers(3, m_VertexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);
	}

	void LoadMesh(std::string Filename);

	void Bind()
	{
		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
	}

	size_t GetIndexCount() { return m_IndexAmount; }
};