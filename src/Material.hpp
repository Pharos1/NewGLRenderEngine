#include "pch.h"
#include "Texture.hpp"

class Material {
public:
	Texture albedo{};
	Texture specular{};
	Texture roughness{};
	Texture normal{};

	Material(const char* albedoPath, const char* specularPath, const char* roughnessPath, const char* normalPath);
	Material();
	void bind(int firstTextureUnit) const;
	void unbind(int firstTextureUnit) const;
};