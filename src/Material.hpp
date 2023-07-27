#pragma once
#include "pch.h"
#include "Texture.hpp"

class Material {
public:
	GLuint albedo = 0;
	GLuint metallic = 0;
	GLuint roughness = 0;
	GLuint normal = 0;

	Material(const char* albedoPath, const char* metallicPath = nullptr, const char* roughnessPath = nullptr, const char* normalPath = nullptr);
	Material();
	~Material();
	void bind(int firstTextureUnit) const;
	void unbind(int firstTextureUnit) const;
	void deleteMaterial();
};