#include "../pch.h"
#include "../Utilities/Logger.hpp"
#include "../Utilities/Time.hpp"
#include "Texture.hpp"

Texture::Texture(const char* path, const TextureCreateInfo& createInfo) {
	loadTexture(path, createInfo);
}
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
	std::swap(type, other.type);
	std::swap(path, other.path);
}
Texture& Texture::operator=(Texture&& other) noexcept {
	if (this == &other) return *this;

	deleteTexture();

	std::swap(id, other.id);
	std::swap(type, other.type);
	std::swap(path, other.path);

	return *this;
}

GLuint Texture::getID() const {
	return this->id;
}
void Texture::bind(const GLuint textureUnit) const {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(type, id);
}
void Texture::unbind() const{
	glBindTexture(type, 0);
}
void Texture::deleteTexture() {
	glDeleteTextures(1, &id);

	id = 0; //Default
	type = GL_TEXTURE_2D;
	path = "";
}
void Texture::loadTexture(const std::string& path, const TextureCreateInfo& createInfo) {
	//Note: glGenTexture generates n number of texture ids and sends them to the second parameter
	//Note: glActiveTexture sets the texture unit that glBindTexture will bind to(starting from 0)
	//Note: glBindTexture sets the texture id(sec parameter) to the texture unit(from glActiveTexture) if glActive texture wasn't called before it is bind to GL_TEXTURE0 
	/*if (!path || path[0] == '\0') { //Null or empty
		mLog("Attempted to load a texture with no path provided! Ensure you entered the correct path.", Log::LogError);
		return;
	}*/

	glActiveTexture(GL_TEXTURE0);

	this->path = path;
	this->type = createInfo.type;
	bool some = empty();
	if (some) glGenTextures(1, &this->id);
	
	unsigned char* data = nullptr;

	int width;
	int height;
	int channels;

	stbi_set_flip_vertically_on_load(createInfo.invertY);

	if (!createInfo.noImage)
		data = stbi_load(this->path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	else {
		width = createInfo.width;
		height = createInfo.height;
		data = (unsigned char*) createInfo.data;
	}
	//this->id = path.empty() ? 0 : SOIL_load_OGL_texture(this->path.c_str(), SOIL_LOAD_RGBA, this->id, invertY ? SOIL_FLAG_INVERT_Y : 0);

	if (data || createInfo.noImage) {
		glBindTexture(createInfo.type, this->id);
		glTexParameteri(createInfo.type, GL_TEXTURE_WRAP_S, createInfo.wrapX); //Note: When using transparency its good to use GL_CLAMP_TO_EDGE instead of GL_REPEAT to prevent interpolation of colors on the top of the texture
		glTexParameteri(createInfo.type, GL_TEXTURE_WRAP_T, createInfo.wrapY); //and here also
		glTexParameteri(createInfo.type, GL_TEXTURE_MIN_FILTER, createInfo.minFilter);
		glTexParameteri(createInfo.type, GL_TEXTURE_MAG_FILTER, createInfo.magFilter);

		glTexImage2D(createInfo.type, 0, createInfo.internalFormat, width, height, 0, createInfo.format, createInfo.dataType, data);
		if(createInfo.generateMipmap)
			glGenerateMipmap(createInfo.type);
	}
	else
		mLog(std::string("Failed to load a texture with path '") + this->path + "'. Last SOIL result: " + SOIL_last_result() + ".", Log::LogError, "TEXTURE");

	glBindTexture(createInfo.type, 0); //Unbind
	if(!createInfo.noImage) stbi_image_free(data);
}
bool Texture::empty() const {
	return !this->id;
}