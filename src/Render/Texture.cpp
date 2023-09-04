#include "../pch.h"
#include "../Utilities/Logger.hpp"
#include "../Utilities/Time.hpp"
#include "Texture.hpp"

Texture::~Texture() { //Made the deletion explicit as this destructor messes everything when there are scopes and copying
	if (!id) {
		//mLog("Attempted to delete an invalid texture ID!", Log::LogWarning);
		return;
	}

	mLog(std::string("~Texture has been triggered! Hint: Texture ID -> ") + std::to_string(this->getID()), Log::LogDestructorInfo, "TEXTURE");
	this->deleteTexture();
}

Texture::Texture(Texture&& other) noexcept {
	std::swap(id, other.id);

	std::swap(target, other.target);
	std::swap(internalFormat, other.internalFormat);
	std::swap(format, other.format);
	std::swap(type, other.type);
	std::swap(filterMin, other.filterMin);
	std::swap(filterMag, other.filterMag);
	std::swap(wrapS, other.wrapS);
	std::swap(wrapT, other.wrapT);
	std::swap(wrapR, other.wrapR);
	std::swap(mipmapping, other.mipmapping);

	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(depth, other.depth);
}
Texture& Texture::operator=(Texture&& other) noexcept {
	if (this == &other) return *this;

	deleteTexture();

	std::swap(id, other.id);

	std::swap(target, other.target);
	std::swap(internalFormat, other.internalFormat);
	std::swap(format, other.format);
	std::swap(type, other.type);
	std::swap(filterMin, other.filterMin);
	std::swap(filterMag, other.filterMag);
	std::swap(wrapS, other.wrapS);
	std::swap(wrapT, other.wrapT);
	std::swap(wrapR, other.wrapR);
	std::swap(mipmapping, other.mipmapping);

	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(depth, other.depth);

	return *this;
}

void Texture::bind(const GLuint unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, id);
}
void Texture::unbind(const GLuint unit) const{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(target, 0);
}
void Texture::deleteTexture() {
	glDeleteTextures(1, &id);

	id = 0; //Default

	target = 0;
	internalFormat = GL_RGBA;
	format = GL_RGBA;
	type = GL_UNSIGNED_BYTE;
	filterMin = GL_LINEAR_MIPMAP_LINEAR;
	filterMag = GL_LINEAR;
	wrapS = GL_REPEAT; //Wrap X
	wrapT = GL_REPEAT; //Wrap Y
	wrapR = GL_REPEAT; //Wrap Z
	mipmapping = true;

	width = 0;
	height = 0;
	depth = 0;
}
bool Texture::empty() const {
	return !this->id;
}

void Texture::create1D(GLenum target, GLuint width, GLenum internalFormat, GLenum format, GLenum type, void* data){
	if (empty()) glGenTextures(1, &id);

	this->width = width;
	this->height = 0;
	this->depth = 0;
	this->internalFormat = internalFormat;
	this->format = format;
	this->type = type;
	this->target = target;

	bind();
	glTexImage1D(target, 0, internalFormat, width, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMag);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
	if (mipmapping)
		glGenerateMipmap(target);
	else
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);

	unbind();
}
void Texture::create2D(GLenum target, GLuint width, GLuint height, GLenum internalFormat, GLenum format, GLenum type, void* data) {
	if (empty()) glGenTextures(1, &id);

	this->width = width;
	this->height = height;
	this->depth = 0;
	this->internalFormat = internalFormat;
	this->format = format;
	this->type = type;
	this->target = target;

	bind();
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMag);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
	if (mipmapping)
		glGenerateMipmap(target);
	else
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);

	unbind();
}
void Texture::create3D(GLenum target, GLuint width, GLuint height, GLuint depth, GLenum internalFormat, GLenum format, GLenum type, void* data) {
	if (empty()) glGenTextures(1, &id);

	this->width = width;
	this->height = height;
	this->depth = depth;
	this->internalFormat = internalFormat;
	this->format = format;
	this->type = type;
	this->target = target;

	bind();
	glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMag);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
	if (mipmapping)
		glGenerateMipmap(target);
	else
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);

	unbind();
}

void Texture::setFilterMin(GLenum filter) {
	bind();
	this->filterMin = filter;
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
}
void Texture::setFilterMag(GLenum filter) {
	bind();
	this->filterMin = filter;
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMag);
}
void Texture::setWrapS(GLenum wrapMethod) {
	bind();
	this->wrapS = wrapMethod;
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
}
void Texture::setWrapT(GLenum wrapMethod) {
	bind();
	this->wrapT = wrapMethod;
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
}
void Texture::setWrapR(GLenum wrapMethod) {
	bind();
	this->wrapR = wrapMethod;
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
}
void Texture::setBorderColor(std::array<float, 4> color) {
	if (empty()) return;
	bind();
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, color.data());
}

GLuint Texture::getID() const { return this->id; }
GLuint Texture::getWidth() const { return width; }
GLuint Texture::getHeight() const { return height; }
GLuint Texture::getDepth() const { return depth; }
GLenum Texture::getTarget() const { return target; }

void Texture::loadSTBI2D(const std::string& path, GLenum internalFormat, GLenum format, GLenum type, bool invertY, GLenum desiredChannels) {
	GLint width, height, channels;

	stbi_set_flip_vertically_on_load(invertY);

	void* data = stbi_load(path.c_str(), &width, &height, &channels, desiredChannels);
	create2D(GL_TEXTURE_2D, width, height, internalFormat, format, type, data);
	stbi_image_free(data);
}