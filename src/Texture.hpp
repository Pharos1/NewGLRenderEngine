#pragma once
#include "pch.h"
#include "Logger.hpp"

class Texture {
protected:
	GLuint id = 0;
	GLuint glType = 0;

public:
	std::string path{};

	Texture(const char* path, bool invertY = false, GLenum glType = GL_TEXTURE_2D);
	Texture() = default;
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	GLuint getID() const;
	void bind(const GLuint textureUnit) const;
	void unbind(const GLuint textureUnit) const;
	void deleteTexture();
	void loadTexture(const char* path, bool invertY = true, GLenum glType = GL_TEXTURE_2D);	

	bool empty() const;
};