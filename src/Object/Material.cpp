#include "../pch.h"
#include "Material.hpp"
#include "../Utilities/Logger.hpp"

void Material::bind() const {
	for (auto& [texture, unit] : textures) {
		if (texture.index() == 0) std::get<std::unique_ptr<Texture>>(texture)->bind(unit);
		else std::get<Texture*>(texture)->bind(unit);
		//(*std::get_if<Texture*>(&texture))->bind(unit);
		//(*std::get_if<std::unique_ptr<Texture>>(&texture))->bind(unit);
	}
}
void Material::unbind() const {
	for (auto& [texture, unit] : textures) {
		if (texture.index() == 0) std::get<std::unique_ptr<Texture>>(texture)->unbind(unit);
		else std::get<Texture*>(texture)->unbind(unit);
	}
}
bool Material::empty() const {
	bool empty = false;

	for (auto& [texture, unit] : textures) {
		if (texture.index() == 0) empty |= std::get<std::unique_ptr<Texture>>(texture)->empty();
		else empty |= std::get<Texture*>(texture)->empty();
	}

	return empty;
}
void Material::deleteMaterial() {
	for (auto& [texture, unit] : textures) {
		if (texture.index() == 0) std::get<std::unique_ptr<Texture>>(texture)->deleteTexture();
	}
	textures.clear();
}
void Material::addTexture(const std::string& path, const GLuint unit) {
	Texture texture;
	texture.loadSTBI2D(path);

	textures.emplace_back(std::make_unique<Texture>(std::move(texture)), unit);
}
void Material::addTexture(Texture* texture, const GLuint unit) {
	textures.emplace_back(texture, unit);
}