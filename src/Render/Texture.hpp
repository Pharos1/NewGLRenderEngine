#pragma once
#include "../pch.h"
#include "../Utilities/Logger.hpp"

class Texture {
private:
	GLuint id = 0;

	GLenum target = GL_TEXTURE_2D;
	GLenum internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;
	GLenum type = GL_UNSIGNED_BYTE;
	GLenum filterMin = GL_LINEAR_MIPMAP_LINEAR;
	GLenum filterMax = GL_LINEAR;
	GLenum wrapS = GL_REPEAT; //Wrap X
	GLenum wrapT = GL_REPEAT; //Wrap Y
	GLenum wrapR = GL_REPEAT; //Wrap Z
	bool mipmapping = true;

	GLuint width = 0;
	GLuint height = 0;
	GLuint depth = 0;

public:
	Texture() = default;
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) noexcept;
	Texture& operator=(Texture&&) noexcept;

	void bind(const GLuint unit = 0) const;
	void unbind() const;
	void deleteTexture();
	bool empty() const;

	void create1D(GLuint width, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, void* data = nullptr);
	void create2D(GLuint width, GLuint height, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, void* data = nullptr);
	void create3D(GLuint width, GLuint height, GLuint depth, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, void* data = nullptr);

	void setFilterMin(GLenum filter);
	void setFilterMax(GLenum filter);
	void setWrapS(GLenum wrapMethod);
	void setWrapT(GLenum wrapMethod);
	void setWrapR(GLenum wrapMethod);

	GLuint getID() const;
	GLuint getWidth() const;
	GLuint getHeight() const;
	GLenum getTarget() const;

	void loadSTBI2D(const std::string& path, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, bool invertY = true, GLenum desiredChannels = STBI_rgb_alpha);
};