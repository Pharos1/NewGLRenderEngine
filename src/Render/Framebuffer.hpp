#pragma once
#include "../pch.h"
#include "../Utilities/Logger.hpp"
#include "../Render/Texture.hpp"

class Framebuffer {
private:
	GLuint FBO = 0;
	GLuint RBO = 0;
	GLuint width = 0;
	GLuint height = 0;
	GLuint depth = 0;
	//GLuint FBOTexture = 0;

public:
	Texture texture;

	Framebuffer() = default;
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;

	void bind(GLuint target = GL_FRAMEBUFFER) const;
	static void unbind(GLuint target = GL_FRAMEBUFFER);
	static void clear();
	void deleteFramebuffer();

	void create1D(GLuint width, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum RBOFormat = GL_DEPTH24_STENCIL8, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum type = GL_UNSIGNED_BYTE);
	void create2D(GLuint width, GLuint height, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum RBOFormat = GL_DEPTH24_STENCIL8, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum type = GL_UNSIGNED_BYTE);
	void create3D(GLuint width, GLuint height, GLuint depth, GLenum internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum RBOFormat = GL_DEPTH24_STENCIL8, GLenum attachment = GL_COLOR_ATTACHMENT0, GLenum type = GL_UNSIGNED_BYTE);
};