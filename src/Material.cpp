#include "pch.h"
#include "Material.hpp"

Material::Material(const char* albedoPath, const char* specularPath, const char* roughnessPath, const char* normalPath) {
	albedo.loadTexture(albedoPath);
	specular.loadTexture(specularPath);
	roughness.loadTexture(roughnessPath);
	normal.loadTexture(normalPath);
}
Material::Material() {};

void Material::bind(int firstTextureUnit) const {
	albedo.bind(firstTextureUnit + 0);
	specular.bind(firstTextureUnit + 1);
	roughness.bind(firstTextureUnit + 2);
	normal.bind(firstTextureUnit + 3);
}
void Material::unbind(int firstTextureUnit) const {
	albedo.bind(firstTextureUnit + 0);
	specular.bind(firstTextureUnit + 1);
	roughness.bind(firstTextureUnit + 2);
	normal.bind(firstTextureUnit + 3);
}