#pragma once
#include "../pch.h"
#include "Shader.hpp"

class DirLight {
private:
	glm::vec3 dir;
	glm::vec3 color;

	bool changed = true;

public:
	bool enabled;

	DirLight(glm::vec3 dir, glm::vec3 color, bool enabled = true);
	DirLight() = default;

	void set(const std::string& objectName, const Shader& shaderProgram);
	void setDir(glm::vec3 value);
	void setColor(glm::vec3 value);

	glm::vec3 getDir() const;
	glm::vec3 getColor() const;
};
class PointLight {
private:
	glm::vec3 pos;
	glm::vec3 color;
	float effectiveRadius;

	bool changed;

public:
	bool enabled = true;

	PointLight(glm::vec3 pos, glm::vec3 color, bool enabled = true);
	PointLight() = default;

	void set(const std::string& objectName, const Shader& shaderProgram);

	void setPos(glm::vec3 value);
	void setColor(glm::vec3 value);

	glm::vec3 getPos() const;
	glm::vec3 getColor() const;
};
class SpotLight {
private:
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 color;
	float effectiveRadius;

	bool changed = true;

public:
	bool enabled;
	float cutOff;
	float outerCutOff;

	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, bool enabled = true);
	SpotLight() = default;

	void set(const std::string& objectName, const Shader& shaderProgram);

	void setPos(glm::vec3 value);
	void setDir(glm::vec3 value);
	void setColor(glm::vec3 value);

	glm::vec3 getDir() const;
	glm::vec3 getPos() const;
	glm::vec3 getColor() const;
};