#pragma once
#include "../pch.h"
#include "../Utilities/Logger.hpp"

class Framebuffer {
private:
	GLuint FBO = 0;
	GLuint RBO = 0;
	GLuint FBOTexture = 0;

public:
	GLenum target = GL_TEXTURE_2D;
	GLsizei width = -1;
	GLsizei height = -1;
	GLint internalFormat = GL_RGBA; //GL_RGBA16F for HDR
	GLint format = GL_RGBA;
	GLint minFilter = GL_LINEAR;
	GLint magFilter = GL_LINEAR;
	GLint RBOFormat = GL_DEPTH24_STENCIL8; //0 for disabled

	Framebuffer(GLsizei width, GLsizei height);
	Framebuffer() = default;
	~Framebuffer();

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	Framebuffer(Framebuffer&& other) noexcept;
	Framebuffer& operator=(Framebuffer&& other) noexcept;
	
	void create();
	void bind() const;
	void bindTexture(GLuint textureUnit) const;
	void unbind() const;
	void unbindTexture(GLuint textureUnit) const;
	void clear() const;
	void deleteFramebuffer();
};