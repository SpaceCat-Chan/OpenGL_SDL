#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"


Texture::Texture(std::string Filename, bool Flip/*=true*/) {
	Load(Filename, Flip);
}

Texture::Texture(Texture&& Move) {
	m_TextureID = Move.m_TextureID;
	Move.m_TextureID = 0;
	m_W = Move.m_W;
	m_H = Move.m_H;
}

const Texture& Texture::operator=(Texture &&Move) {
	m_TextureID = Move.m_TextureID;
	Move.m_TextureID = 0;
	m_W = Move.m_W;
	m_H = Move.m_H;

	return *this;
}

bool Texture::Load(std::string Filename, bool Flip/*=true*/) {
	stbi_set_flip_vertically_on_load(Flip);
	unsigned char *Image = stbi_load(Filename.c_str(), &m_W, &m_H, nullptr, STBI_rgb_alpha);
	if(Image) {
		Destroy();
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_W, m_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(Image);
		return true;
	}
	else {
		stbi_image_free(Image);
		return false;
	}
}

void Texture::Bind(size_t Position/*=0*/) {
	glActiveTexture(GL_TEXTURE0 + Position);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::Destroy() {
	if (m_TextureID == 0) return;
	glDeleteTextures(1, &m_TextureID);
}