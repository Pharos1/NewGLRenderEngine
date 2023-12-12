#include "../pch.h"
#include "Light.hpp"

static float calcLightRadius(const glm::vec3 color) {
	float maxIntensity = glm::max(glm::max(color.x, color.y), color.z);
	float radius = 16 * glm::sqrt(maxIntensity);
	radius /= glm::sqrt(5);

	return radius;
}

//DirLight
DirLight::DirLight(glm::vec3 dir, glm::vec3 color, bool enabled)
	: dir(glm::normalize(dir)), color(color), enabled(enabled) {
}

void DirLight::set(const std::string& objectName, const Shader& shaderProgram) {
	if (!changed) return;

	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".dir", dir);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1b(objectName + ".enabled", enabled);

	changed = false;
}
void DirLight::setDir(glm::vec3 value) {
	dir = glm::normalize(value);
	changed = true;
}
void DirLight::setColor(glm::vec3 value) {
	color = value;
	changed = true;
}

glm::vec3 DirLight::getDir() const { return dir; }
glm::vec3 DirLight::getColor() const { return color; }

//PointLight
PointLight::PointLight(glm::vec3 pos, glm::vec3 color, bool enabled)
	: pos(pos), color(color), enabled(enabled) {
}

void PointLight::set(const std::string& objectName, const Shader& shaderProgram) {
	if (!changed) return;

	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".pos", pos);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1f(objectName + ".effectiveRadius", effectiveRadius);
	shaderProgram.set1b(objectName + ".enabled", enabled);

	changed = false;
}
void PointLight::setPos(glm::vec3 value) {
	pos = value;
	changed = true;
}
void PointLight::setColor(glm::vec3 value) {
	effectiveRadius = calcLightRadius(color);
	color = value;
	changed = true;
}

glm::vec3 PointLight::getPos() const { return pos; }
glm::vec3 PointLight::getColor() const { return color; }

//SpotLight
SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, bool enabled)
	: pos(pos), dir(glm::normalize(dir)), color(color), cutOff(cutOff), outerCutOff(outerCutOff), enabled(enabled) {
}

void SpotLight::set(const std::string& objectName, const Shader& shaderProgram) {
	if (!changed) return;

	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".pos", pos);
	shaderProgram.setVec3(objectName + ".dir", dir);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1f(objectName + ".cutOff", glm::cos(glm::radians(cutOff)));
	shaderProgram.set1f(objectName + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));
	shaderProgram.set1f(objectName + ".effectiveRadius", effectiveRadius);
	shaderProgram.set1b(objectName + ".enabled", enabled);

	changed = false;
}
void SpotLight::setDir(glm::vec3 value) {
	dir = glm::normalize(value);
	changed = true;
}
void SpotLight::setPos(glm::vec3 value) {
	pos = value;
	changed = true;
}
void SpotLight::setColor(glm::vec3 value) {
	effectiveRadius = calcLightRadius(color);
	color = value;
	changed = true;
}

glm::vec3 SpotLight::getDir() const { return dir; }
glm::vec3 SpotLight::getPos() const { return pos; }
glm::vec3 SpotLight::getColor() const { return color; }