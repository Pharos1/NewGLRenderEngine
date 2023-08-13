#pragma once
#include "../pch.h"
#include "../Utilities/Logger.hpp"

struct TextureCreateInfo {
	GLuint type = GL_TEXTURE_2D;
	GLuint wrapX = GL_REPEAT;
	GLuint wrapY = GL_REPEAT;
	GLuint minFilter = GL_LINEAR_MIPMAP_LINEAR;
	GLuint magFilter = GL_LINEAR;
	GLuint internalFormat = GL_RGBA;
	GLuint format = GL_RGBA;
	GLuint dataType = GL_UNSIGNED_BYTE;
	bool invertY = true;
	int width = 0;
	int height = 0;
	void* data = nullptr;
	bool noImage = false;
	bool generateMipmap = true;
};
class Texture {
private:
	GLuint id = 0;
	GLuint type = GL_TEXTURE_2D;

public:
	std::string path{};

	Texture(const char* path, const TextureCreateInfo& createInfo);
	Texture() = default;
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	GLuint getID() const;
	void bind(const GLuint textureUnit) const;
	void unbind() const;
	void deleteTexture();
	void loadTexture(const std::string& path, const TextureCreateInfo& createInfo = TextureCreateInfo());

	bool empty() const;
};