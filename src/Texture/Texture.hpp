#pragma once

#include <iostream>

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/ext.hpp>

#include "stb_image.h"

/**
 * \brief a class that manages a texture
 * 
 * copying has been deleted
 */
class Texture {
    GLuint m_TextureID = 0;
	int m_W=0, m_H=0;

	public:

	Texture() = default;
	~Texture() {
		Destroy();
	}
	Texture(std::string Filename);
	Texture(const Texture&) = delete;
	Texture(Texture&&);

	const Texture& operator=(const Texture&) = delete;
	const Texture& operator=(Texture&&);

	/**
	 * \brief loads a Texture from a file
	 * 
	 * \param Filename the name of the file to load from
	 * 
	 * \return weather or not the loading succeded
	 * 
	 * if loading the file failed then the Texture will be unchanged
	 */
	bool Load(std::string Filename);

	/**
	 * \brief binds the texture
	 * 
	 * \param Position the position to load the texture into, defaults to 0
	 */
	void Bind(size_t Position=0);

	/**
	 * \brief destroys the contained texture
	 */
	void Destroy();
};