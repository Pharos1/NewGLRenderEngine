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
	GLuint getID() const;

	void bind(const GLint textureUnit) const;
	void unbind(const GLint texture_unit) const;
	void deleteTexture();
	void loadTexture(const char* path, bool invertY = true, GLenum glType = GL_TEXTURE_2D);

	Texture(const char* path, bool invertY = false, GLenum glType = GL_TEXTURE_2D);
	Texture();
	~Texture();
};