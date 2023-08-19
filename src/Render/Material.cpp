#include "../pch.h"
#include "Material.hpp"
#include "../Utilities/Logger.hpp"

void Material::bind() const {
	for (auto& [texture, unit] : textures) {
		texture.bind(unit);
	}
}
void Material::unbind() const {
	for (auto& [texture, unit] : textures) {
		texture.unbind();
	}
}
bool Material::empty() const {
	bool empty = false;

	for (auto& [texture, unit] : textures) {
		empty |= texture.empty();
	}

	return empty;
}
void Material::deleteMaterial() {
	for (auto& [texture, unit] : textures) {
		texture.deleteTexture();
	}
	textures.clear();
}
void Material::addTexture(const std::string& path, const GLuint unit) {
	Texture texture;
	texture.loadSTBI2D(path);

	textures.emplace_back(std::move(texture), unit);
}