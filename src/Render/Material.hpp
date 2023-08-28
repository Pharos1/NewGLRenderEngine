#pragma once
#include "../pch.h"
#include "Texture.hpp"

class Material {
public:
	std::vector<std::pair<std::variant<std::unique_ptr<Texture>, Texture*>, GLuint>> textures; //Note/Todo: Very wacky system. Does the job done, but it is messy and it would be better if I can come up with cleaner solution

	Material() = default;

	void bind() const;
	void unbind() const;
	bool empty() const;
	void deleteMaterial();

	void addTexture(const std::string& path, const GLuint unit = 0);
	void addTexture(Texture* texture, const GLuint unit = 0);
	//Todo: maybe a removeTexture func
};