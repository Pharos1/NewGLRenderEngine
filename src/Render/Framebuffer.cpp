#include "../pch.h"
#include "Framebuffer.hpp"

Framebuffer::~Framebuffer() {
	nLog(std::string("~Framebuffer has been triggered! Hint: FBO ID -> ") + std::to_string(this->FBO), Log::LogDestructorInfo, "FRAMEBUFFER");
	deleteFramebuffer();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
	std::swap(FBO, other.FBO);
	std::swap(RBO, other.RBO);
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(depth, other.depth);
}
Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
	if (this == &other) return *this;

	deleteFramebuffer();

	std::swap(FBO, other.FBO);
	std::swap(RBO, other.RBO);
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(depth, other.depth);

	return *this;
}

void Framebuffer::bind(GLuint target) const {
	glBindFramebuffer(target, FBO);
}
void Framebuffer::unbind(GLuint target) {
	glBindFramebuffer(target, 0);
}
void Framebuffer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Framebuffer::deleteFramebuffer() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);

	FBO = 0; //Default
	RBO = 0;
	width = 0;
	height = 0;
	depth = 0;
}

void Framebuffer::create1D(GLuint width, GLenum internalFormat, GLenum format, GLenum RBOFormat, GLenum attachment, GLenum type) {
	width = width;
	height = 0;
	depth = 0;
	
	if (!FBO) glGenFramebuffers(1, &FBO);
	if (!RBO && RBOFormat) glGenRenderbuffers(1, &RBO);

	texture.mipmapping = false;
	texture.setFilterMin(GL_LINEAR);
	texture.setFilterMag(GL_LINEAR);
	texture.create1D(width, internalFormat, format, type, nullptr);

	bind();
	texture.bind();

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.getTarget(), texture.getID(), 0);

	if (RBOFormat) {
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, RBOFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		mLog("Failed to create framebuffer!", Log::LogError, "FRAMEBUFFER");

	unbind();
	texture.unbind();
}
void Framebuffer::create2D(GLuint width, GLuint height, GLenum internalFormat, GLenum format, GLenum RBOFormat, GLenum attachment, GLenum type) {
	width = width;
	height = height;
	depth = 0;
	
	if (!FBO) glGenFramebuffers(1, &FBO);
	if (!RBO && RBOFormat) glGenRenderbuffers(1, &RBO);

	texture.mipmapping = false;
	texture.setFilterMin(GL_LINEAR);
	texture.setFilterMag(GL_LINEAR);
	texture.create2D(width, height, internalFormat, format, type, nullptr);

	bind();
	texture.bind();

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.getTarget(), texture.getID(), 0);

	if (RBOFormat) {
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, RBOFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		mLog("Failed to create framebuffer!", Log::LogError, "FRAMEBUFFER");

	unbind();
	texture.unbind();
}
void Framebuffer::create3D(GLuint width, GLuint height, GLuint depth, GLenum internalFormat, GLenum format, GLenum RBOFormat, GLenum attachment, GLenum type) {
	width = width;
	height = height;
	depth = depth;
	
	if (!FBO) glGenFramebuffers(1, &FBO);
	if (!RBO && RBOFormat) glGenRenderbuffers(1, &RBO);

	texture.mipmapping = false;
	texture.setFilterMin(GL_LINEAR);
	texture.setFilterMag(GL_LINEAR);
	texture.create3D(width, height, depth, internalFormat, format, type, nullptr);

	bind();
	texture.bind();

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.getTarget(), texture.getID(), 0);

	if (RBOFormat) {
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, RBOFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		mLog("Failed to create framebuffer!", Log::LogError, "FRAMEBUFFER");

	unbind();
	texture.unbind();
}