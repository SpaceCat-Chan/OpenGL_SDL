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
 * \brief Class for loading and handeling meshes
 * 
 * meshes are loaded from obj files
 */
class Mesh
{
	GLuint m_VertexArray = 0;
	GLuint m_VertexBuffer[8];
	std::vector<GLuint> m_IndexBuffer;
	std::vector<size_t> m_IndexAmount;



public:
	Mesh()
	{
		glGenBuffers(8, m_VertexBuffer);
		glGenVertexArrays(1, &m_VertexArray);
	}
	~Mesh()
	{
		glDeleteBuffers(8, m_VertexBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);

		for(size_t i=0; i<m_IndexBuffer.size(); i++) {
			glDeleteBuffers(1, &m_IndexBuffer[i]);
		}
	}

	/**
	 * \brief a function that loads a mesh
	 * 
	 * \param Filename the name of the file to load from
	 * 
	 * there is currently no way to access the stored mesh data without major slowdown or changes to this class
	 */
	void LoadMesh(std::string Filename);

	void Bind(size_t MeshIndex)
	{
		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer[MeshIndex]);
	}

	size_t GetIndexCount(size_t MeshIndex) { return m_IndexAmount[MeshIndex]; }
};