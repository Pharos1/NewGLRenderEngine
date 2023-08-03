#include "../pch.h"
#include "../Utilities/Logger.hpp"
#include "Texture.hpp"

Texture::Texture(const char* path, bool invertY, GLenum glType) {
	loadTexture(path, invertY, glType);
}
Texture::~Texture() { //Made the deletion explicit as this destructor messes everything when there are scopes and copying
	if (!id) {
		//mLog("Attempted to delete an invalid texture ID!", Log::LogWarning);
		return;
	}

	mLog(std::string("~Texture has been triggered! Hint: Texture ID -> ") + std::to_string(this->getID()), Log::LogDestructorInfo);
	this->deleteTexture();
}

Texture::Texture(Texture&& other) noexcept {
	std::swap(id, other.id);
	std::swap(glType, other.glType);
	std::swap(path, other.path);
}
Texture& Texture::operator=(Texture&& other) noexcept {
	if (this == &other) return *this;

	deleteTexture();

	std::swap(id, other.id);
	std::swap(glType, other.glType);
	std::swap(path, other.path);

	return *this;
}

GLuint Texture::getID() const { return this->id; }
void Texture::bind(const GLuint textureUnit) const {
	if (!id) {
		//mLog("Attempted to bind an invalid texture ID!", Log::LogWarning);
		return;
	}

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(glType, id);
}
void Texture::unbind(const GLuint textureUnit) const {
	if (!id) {
		//mLog("Attempted to unbind an invalid texture ID!", Log::LogWarning);
		return;
	}

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(glType, 0);
}
void Texture::deleteTexture() {
	glDeleteTextures(1, &id);

	id = 0; //Just in case
	glType = 0;
	path = "";
}
void Texture::loadTexture(const char* path, bool invertY, GLenum glType) {
	//Note: glGenTexture generates n number of texture ids and sends them to the second parameter
	//Note: glActiveTexture sets the texture unit that glBindTexture will bind to(starting from 0)
	//Note: glBindTexture sets the texture id(sec parameter) to the texture unit(from glActiveTexture) if glActive texture wasn't called before it is bind to GL_TEXTURE0 
	if (!path || path[0] == '\0') { //Null or empty
		mLog("Attempted to load a texture with no path provided! Ensure you entered the correct path.", Log::LogError);
		return;
	}

	glActiveTexture(GL_TEXTURE0);

	this->path = path;
	this->glType = glType;

	if (empty()) glGenTextures(1, &this->id);
	bool data = SOIL_load_OGL_texture(path, SOIL_LOAD_RGBA, this->id, invertY ? SOIL_FLAG_INVERT_Y : 0);

	if (data) {
		glTexParameteri(this->glType, GL_TEXTURE_WRAP_S, GL_REPEAT); //Note: When using transparency its good to use GL_CLAMP_TO_EDGE instead of GL_REPEAT to prevent interpolation of colors on the top of the texture
		glTexParameteri(this->glType, GL_TEXTURE_WRAP_T, GL_REPEAT); //and here also
		glTexParameteri(this->glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(this->glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenerateMipmap(this->glType);
	}
	else
		mLog(std::string("Failed to load a texture with path '") + this->path + "'. Last SOIL result: " + SOIL_last_result(), Log::LogError);

	glBindTexture(this->glType, 0); //Unbind
}
bool Texture::empty() const {
	return !this->id;
}