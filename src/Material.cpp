#include "pch.h"
#include "Material.hpp"
#include "Logger.hpp"

Material::Material(const char* albedoPath, const char* metallicPath, const char* roughnessPath, const char* normalPath) {
	albedo = TextureNS::loadTexture(albedoPath);
	if (metallicPath) metallic = TextureNS::loadTexture(metallicPath);
	if (metallicPath) roughness = TextureNS::loadTexture(roughnessPath);
	if (metallicPath) normal = TextureNS::loadTexture(normalPath);
}
Material::Material() {};
Material::~Material() {
	mLog(std::string("The destructor of class Material has been triggered! Ensure that all resources are properly handled. Hint: Albedo Texture ID -> ") + std::to_string(albedo), Log::LogWarning);
}

void Material::bind(int firstTextureUnit) const {
	TextureNS::bind(albedo, firstTextureUnit + 0);
	TextureNS::bind(metallic, firstTextureUnit + 1);
	TextureNS::bind(roughness, firstTextureUnit + 2);
	TextureNS::bind(normal, firstTextureUnit + 3);
}
void Material::unbind(int firstTextureUnit) const {
	TextureNS::unbind(albedo, firstTextureUnit + 0);
	TextureNS::unbind(metallic, firstTextureUnit + 1);
	TextureNS::unbind(roughness, firstTextureUnit + 2);
	TextureNS::unbind(normal, firstTextureUnit + 3);
}
void Material::deleteMaterial() {
	TextureNS::deleteTexture(albedo);
	TextureNS::deleteTexture(metallic);
	TextureNS::deleteTexture(roughness);
	TextureNS::deleteTexture(normal);
}