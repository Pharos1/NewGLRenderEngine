#pragma once
#include "pch.h"
#include "Shader.hpp"

class DirLight {
public:
	glm::vec3 dir;
	glm::vec3 color;

	DirLight(glm::vec3 dir, glm::vec3 color);
	DirLight() = default;
	void set(const std::string& objectName, Shader& shaderProgram) const;
};
class PointLight {
public:
	glm::vec3 pos;
	glm::vec3 color;

	PointLight(glm::vec3 pos, glm::vec3 color);
	PointLight() = default;
	void set(const std::string& objectName, Shader& shaderProgram) const;
};
class SpotLight {
public:
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 color;
	float cutOff;
	float outerCutOff;

	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff);
	SpotLight() = default;
	void set(const std::string& objectName, Shader& shaderProgram) const;
};