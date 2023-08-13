#include "../pch.h"
#include "Framebuffer.hpp"

Framebuffer::Framebuffer(GLsizei width, GLsizei height)
	: width(width), height(height) {
	create();
}
Framebuffer::~Framebuffer() {
	mLog(std::string("~Framebuffer has been triggered! Hint: FBO ID -> ") + std::to_string(this->FBO), Log::LogDestructorInfo, "FRAMEBUFFER");
	deleteFramebuffer();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
	std::swap(FBO, other.FBO);
	std::swap(RBO, other.RBO);
	std::swap(FBOTexture, other.FBOTexture);
	//std::swap(target, other.target);
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(internalFormat, other.internalFormat);
	std::swap(format, other.format);
	//std::swap(minFilter, other.minFilter);
	//std::swap(magFilter, other.magFilter);
	std::swap(RBOFormat, other.RBOFormat);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
	if (this == &other) return *this;

	deleteFramebuffer();

	std::swap(FBO, other.FBO);
	std::swap(RBO, other.RBO);
	std::swap(FBOTexture, other.FBOTexture);
	//std::swap(target, other.target);
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(internalFormat, other.internalFormat);
	std::swap(format, other.format);
	//std::swap(minFilter, other.minFilter);
	//std::swap(magFilter, other.magFilter);
	std::swap(RBOFormat, other.RBOFormat);
}

void Framebuffer::create() {
	if (width < 0 || height < 0) {
		mLog("Attempted to create a framebuffer with no size provided! Ensure you entered the correct width and height.", Log::LogError, "FRAMEBUFFER");
		return;
	}

	if (!FBO) glGenFramebuffers(1, &FBO);
	if (!RBO) glGenRenderbuffers(1, &RBO);
	//if (!FBOTexture) glGenTextures(1, &FBOTexture);
	TextureCreateInfo createInfo;
	createInfo.wrapX = GL_CLAMP_TO_BORDER;
	createInfo.wrapY = GL_CLAMP_TO_BORDER;
	createInfo.minFilter = minFilter;
	createInfo.magFilter = magFilter;
	createInfo.width = width;
	createInfo.height = height;
	createInfo.noImage = true;
	createInfo.internalFormat = internalFormat;
	createInfo.format = format;
	createInfo.generateMipmap = false;
	createInfo.dataType = dataType;

	FBOTexture.loadTexture("", createInfo);


	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	FBOTexture.bind(0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, useTextureAs, GL_TEXTURE_2D, FBOTexture.getID(), 0);

	if (RBOFormat) {
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, RBOFormat, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		mLog("Failed to create a framebuffer!", Log::LogError, "FRAMEBUFFER");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	FBOTexture.unbind();
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

	FBO = 0; //On delete default everything
	RBO = 0;
	width = -1;
	height = -1;
	internalFormat = GL_RGBA;
	format = GL_RGBA;
	minFilter = GL_LINEAR;
	magFilter = GL_LINEAR;
	RBOFormat = GL_DEPTH24_STENCIL8;
}