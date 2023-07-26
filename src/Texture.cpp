#include "pch.h"
#include "Logger.hpp"
#include "Texture.hpp"

GLuint Texture::getID() const { return this->id; }
void Texture::bind(const GLint textureUnit) const {
	if (!this->id) {
		//mLog("Attempted to bind an invalid texture ID!", Log::LogWarning);
		return;
	}

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(this->glType, this->id);
}
void Texture::unbind(const GLint texture_unit) const {
	if (!this->id) {
		//mLog("Attempted to unbind an invalid texture ID!", Log::LogWarning);
		return;
	}

	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(this->glType, 0);
}
void Texture::deleteTexture() {
	glDeleteTextures(1, &this->id);
}
void Texture::loadTexture(const char* path, bool invertY, GLenum glType) {
	//Note: glGenTexture generates n number of texture ids and sends them to the second parameter
	//Note: glActiveTexture sets the texture unit that glBindTexture will bind to(starting from 0)
	//Note: glBindTexture sets the texture id(sec parameter) to the texture unit(from glActiveTexture) if glActive texture wasn't called before it is bind to GL_TEXTURE0 
	if (!path || path[0] == '\0') { //Null or empty
		mLog("Attempted to load a texture with no path provided!", Log::LogError);
		return;
	}

	glActiveTexture(GL_TEXTURE0);

	//this->path = path;
	this->glType = glType;

	if (!this->id) glGenTextures(1, &id);
	bool data = SOIL_load_OGL_texture(path, SOIL_LOAD_RGBA, this->id, invertY ? SOIL_FLAG_INVERT_Y : 0);

	if (data) {
		glTexParameteri(glType, GL_TEXTURE_WRAP_S, GL_REPEAT); //Note: When using transparency its good to use GL_CLAMP_TO_EDGE instead of GL_REPEAT to prevent interpolation of colors on the top of the texture
		glTexParameteri(glType, GL_TEXTURE_WRAP_T, GL_REPEAT); //and here also
		glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenerateMipmap(glType);
	}
	else
		mLog(std::string("Failed to load a texture with path '") + path + "'. Last SOIL result: " + SOIL_last_result(), Log::LogError);

	glBindTexture(glType, 0); //Unbind
}

Texture::Texture(const char* path, bool invertY, GLenum glType) {
	loadTexture(path, invertY, glType);
}
Texture::Texture() {};
Texture::~Texture() {
	if (!id) {
		mLog("Attempted to delete an invalid texture ID!", Log::LogWarning);
		return;
	}

	mLog(std::string("The destructor of texture with ID ") + std::to_string(id) + " got invoked.", Log::LogWarning);
	this->deleteTexture();
}