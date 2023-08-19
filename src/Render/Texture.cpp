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
	std::swap(filterMax, other.filterMax);
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
	std::swap(filterMax, other.filterMax);
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
void Texture::unbind() const{
	glBindTexture(target, 0);
}
void Texture::deleteTexture() {
	glDeleteTextures(1, &id);

	id = 0; //Default

	target = GL_TEXTURE_2D;
	internalFormat = GL_RGBA;
	format = GL_RGBA;
	type = GL_UNSIGNED_BYTE;
	filterMin = GL_LINEAR_MIPMAP_LINEAR;
	filterMax = GL_LINEAR;
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

void Texture::create1D(GLuint width, GLenum internalFormat, GLenum format, GLenum type, void* data){
	glGenTextures(1, &id);

	width = width;
	height = 0;
	depth = 0;
	internalFormat = internalFormat;
	format = format;
	type = type;

	if (target != GL_TEXTURE_1D)
		nLog("Incorrect target used!", Log::LogError, "Texture::create1D");
	
	bind();
	glTexImage1D(target, 0, internalFormat, width, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMax);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	if (mipmapping)
		glGenerateMipmap(target);
	unbind();
}
void Texture::create2D(GLuint width, GLuint height, GLenum internalFormat, GLenum format, GLenum type, void* data) {
	glGenTextures(1, &id);

	width = width;
	height = height;
	depth = 0;
	internalFormat = internalFormat;
	format = format;
	type = type;

	if (target != GL_TEXTURE_2D)
		nLog("Incorrect target used!", Log::LogError, "Texture::create2D");

	bind();
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMax);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	if (mipmapping)
		glGenerateMipmap(target);
	unbind();
}
void Texture::create3D(GLuint width, GLuint height, GLuint depth, GLenum internalFormat, GLenum format, GLenum type, void* data) {
	glGenTextures(1, &id);

	width = width;
	height = height;
	depth = depth;
	internalFormat = internalFormat;
	format = format;
	type = type;

	if (target != GL_TEXTURE_3D)
		nLog("Incorrect target used!", Log::LogError, "Texture::create3D");

	bind();
	glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, data);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMax);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	if (mipmapping)
		glGenerateMipmap(target);
	unbind();
}

void Texture::setFilterMin(GLenum filter) {
	bind();
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
}
void Texture::setFilterMax(GLenum filter) {
	bind();
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);
}
void Texture::setWrapS(GLenum wrapMethod) {
	bind();
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMethod);
}
void Texture::setWrapT(GLenum wrapMethod) {
	bind();
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMethod);
}
void Texture::setWrapR(GLenum wrapMethod) {
	bind();
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMethod);
}

GLuint Texture::getID() const { return this->id; }
GLuint Texture::getWidth() const { return width; }
GLuint Texture::getHeight() const { return height; }
GLenum Texture::getTarget() const { return target; };

void Texture::loadSTBI2D(const std::string& path, GLenum internalFormat, GLenum format, GLenum type, bool invertY, GLenum desiredChannels) {
	GLint width, height, channels;

	stbi_set_flip_vertically_on_load(invertY);

	void* data = stbi_load(path.c_str(), &width, &height, &channels, desiredChannels);
	create2D(width, height, internalFormat, format, type, data);
	stbi_image_free(data);
}