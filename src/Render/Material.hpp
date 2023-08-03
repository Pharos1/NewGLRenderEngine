#pragma once
#include "../pch.h"
#include "Texture.hpp"

class Material {
public:
	Texture albedo;
	Texture metallic;
	Texture roughness;
	Texture normal;

	Material(const char* albedoPath, const char* metallicPath = nullptr, const char* roughnessPath = nullptr, const char* normalPath = nullptr);
	Material() = default;

	void bind(int firstTextureUnit) const;
	void unbind(int firstTextureUnit) const;
	void deleteMaterial();

	bool empty() const;
};