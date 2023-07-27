#pragma once
#include "pch.h"
#include "Logger.hpp"

class Texture {
protected:
	int width = 0;
	int height = 0;
	int channels = 0;

	GLuint id = 0;
	GLuint glType = 0;

public:
	std::string path{};

	GLuint getID() const;

	void bind(const GLuint textureUnit) const;
	void unbind(const GLuint textureUnit) const;
	void deleteTexture();
	void loadTexture(const char* path, bool invertY = true, GLenum glType = GL_TEXTURE_2D);

	Texture(const char* path, bool invertY = false, GLenum glType = GL_TEXTURE_2D);
	Texture();
	~Texture();
	//Delete the copy constructor/assignment.
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&);
	//Texture& operator=(Texture&&);
};
namespace TextureNS {
	void bind(uint32_t id, const GLuint textureUnit, const GLuint = GL_TEXTURE_2D);
	void unbind(uint32_t id, const GLuint textureUnit, const GLuint = GL_TEXTURE_2D);
	void deleteTexture(uint32_t& id);
	GLuint loadTexture(const char* path, bool invertY = true, GLenum glType = GL_TEXTURE_2D);
};