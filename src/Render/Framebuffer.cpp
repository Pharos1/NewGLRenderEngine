#include "../pch.h"
#include "Framebuffer.hpp"

Framebuffer::Framebuffer(GLsizei width, GLsizei height)
	: width(width), height(height) {
	create();
}
Framebuffer::~Framebuffer() {
	mLog(std::string("~Framebuffer has been triggered! Hint: FBO ID -> ") + std::to_string(this->FBO), Log::LogDestructorInfo);
	deleteFramebuffer();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
	std::swap(FBO, other.FBO);
	std::swap(RBO, other.RBO);
	std::swap(FBOTexture, other.FBOTexture);
	std::swap(target, other.target);
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(internalFormat, other.internalFormat);
	std::swap(format, other.format);
	std::swap(minFilter, other.minFilter);
	std::swap(magFilter, other.magFilter);
	std::swap(RBOFormat, other.RBOFormat);
}
Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
	if (this == &other) return *this;

	deleteFramebuffer();

	std::swap(FBO, other.FBO);
	std::swap(RBO, other.RBO);
	std::swap(FBOTexture, other.FBOTexture);
	std::swap(target, other.target);
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(internalFormat, other.internalFormat);
	std::swap(format, other.format);
	std::swap(minFilter, other.minFilter);
	std::swap(magFilter, other.magFilter);
	std::swap(RBOFormat, other.RBOFormat);
}

void Framebuffer::create() {
	if (width < 0 || height < 0) {
		mLog("Attempted to create a framebuffer with no size provided! Ensure you entered the correct width and height.", Log::LogError);
		return;
	}

	if (!FBO) glGenFramebuffers(1, &FBO);
	if (!RBO) glGenRenderbuffers(1, &RBO);
	if (!FBOTexture) glGenTextures(1, &FBOTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glBindTexture(target, FBOTexture);

	glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, FBOTexture, 0);

	if (RBOFormat) {
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, RBOFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		mLog("Failed to create a framebuffer!", Log::LogError);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(target, 0);
}
void Framebuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}
void Framebuffer::bindTexture(const GLuint textureUnit) const {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(target, FBOTexture);
}
void Framebuffer::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Framebuffer::unbindTexture(const GLuint textureUnit) const {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(target, 0);
}
void Framebuffer::clear() const {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void Framebuffer::deleteFramebuffer() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
	glDeleteTextures(1, &FBOTexture);

	FBO = 0; //On delete default everything
	RBO = 0;
	FBOTexture = 0;
	target = GL_TEXTURE_2D;
	width = -1;
	height = -1;
	internalFormat = GL_RGBA;
	format = GL_RGBA;
	minFilter = GL_LINEAR;
	magFilter = GL_LINEAR;
	RBOFormat = GL_DEPTH24_STENCIL8;
}