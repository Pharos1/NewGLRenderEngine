#pragma once
#include "../pch.h"
#include "Texture.hpp"

class Material {
public:
	std::vector<std::pair<Texture, GLuint>> textures;

	Material() = default;

	void bind() const;
	void unbind() const;
	bool empty() const;
	void deleteMaterial();

	void addTexture(const std::string& path, const GLuint unit = 0);
	//Todo: maybe a removeTexture func
};