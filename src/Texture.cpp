#include "pch.h"
#include "Logger.hpp"
#include "Texture.hpp"

Texture::Texture(const char* path, bool invertY, GLenum glType) {
	loadTexture(path, invertY, glType);
}
Texture::Texture() {};
Texture::~Texture() { //Made the deletion explicit as this destructor messes everything when there are scopes and copying
	if (!id) {
		mLog("Attempted to delete an invalid texture ID!", Log::LogWarning);
		return;
	}

	mLog(std::string("The destructor of class Texture has been triggered! Ensure that all resources are properly handled. Hint: Texture ID -> ") + std::to_string(this->getID()), Log::LogWarning);
	this->deleteTexture();
}

GLuint Texture::getID() const { return this->id; }
void Texture::bind(const GLuint textureUnit) const {
	TextureNS::bind(this->id, textureUnit);
}
void Texture::unbind(const GLuint textureUnit) const {
	TextureNS::unbind(this->id, textureUnit);
}
void Texture::deleteTexture() {
	TextureNS::deleteTexture(this->id);
	this->id = 0;
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

	if (!this->id) glGenTextures(1, &this->id);
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

Texture::Texture(Texture&& other) : id(other.id) {
	//other.id = 0; //Use the "null" texture for the old object.
	std::swap(id, other.id);
}

namespace TextureNS {
	void bind(uint32_t id, const GLuint textureUnit, GLuint glType) {
		if (!id) {
			//mLog("Attempted to bind an invalid texture ID!", Log::LogWarning);
			return;
		}

		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(glType, id);
	}
	void unbind(uint32_t id, const GLuint textureUnit, GLuint glType) {
		if (!id) {
			//mLog("Attempted to unbind an invalid texture ID!", Log::LogWarning);
			return;
		}

		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(glType, 0);
	}
	void deleteTexture(uint32_t& id){
		glDeleteTextures(1, &id);
		id = 0; //Just in case
	}
	GLuint loadTexture(const char* path, bool invertY, GLenum glType) {
		if (!path || path[0] == '\0') { //Null or empty
			mLog("Attempted to load a texture with no path provided! Ensure you entered the correct path.", Log::LogError);
			return 0;
		}

		glActiveTexture(GL_TEXTURE0);

		GLuint id = SOIL_load_OGL_texture(path, SOIL_LOAD_RGBA, 0, invertY ? SOIL_FLAG_INVERT_Y : 0);

		if (id) {
			glTexParameteri(glType, GL_TEXTURE_WRAP_S, GL_REPEAT); //Note: When using transparency its good to use GL_CLAMP_TO_EDGE instead of GL_REPEAT to prevent interpolation of colors on the top of the texture
			glTexParameteri(glType, GL_TEXTURE_WRAP_T, GL_REPEAT); //and here also
			glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			glGenerateMipmap(glType);
		}
		else
			mLog(std::string("Failed to load a texture with path '") + path + "'. Last SOIL result: " + SOIL_last_result(), Log::LogError);

		glBindTexture(glType, 0); //Unbind

		return id;
	}
};