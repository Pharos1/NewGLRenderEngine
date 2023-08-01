#include "pch.h"
#include "Material.hpp"
#include "Logger.hpp"

Material::Material(const char* albedoPath, const char* metallicPath, const char* roughnessPath, const char* normalPath) {
	if (albedoPath) albedo.loadTexture(albedoPath);
	if (metallicPath) metallic.loadTexture(metallicPath);
	if (roughnessPath) roughness.loadTexture(roughnessPath);
	if (normalPath) normal.loadTexture(normalPath);
}

void Material::bind(int firstTextureUnit) const {
	albedo.bind(firstTextureUnit);
	metallic.bind(firstTextureUnit + 1);
	roughness.bind(firstTextureUnit + 2);
	normal.bind(firstTextureUnit + 3);
}
void Material::unbind(int firstTextureUnit) const {
	albedo.unbind(firstTextureUnit);
	metallic.unbind(firstTextureUnit + 1);
	roughness.unbind(firstTextureUnit + 2);
	normal.unbind(firstTextureUnit + 3);
}
void Material::deleteMaterial() {
	mLog(std::string("Function 'deleteMaterial()' of class Material has been triggered! Ensure that all resources are properly handled. Hint: Albedo Texture ID -> ") + std::to_string(albedo.getID()), Log::LogInfo);
	
	albedo.deleteTexture();
	metallic.deleteTexture();
	roughness.deleteTexture();
	normal.deleteTexture();
}

bool Material::empty() const {
	if (!albedo.empty()) return false;
	else if (!metallic.empty()) return false;
	else if (!roughness.empty()) return false;
	else if (!normal.empty()) return false;

	return true;
}