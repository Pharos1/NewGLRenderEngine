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
	: dir(dir), color(color), enabled(enabled) {
}

void DirLight::set(const std::string& objectName, const Shader& shaderProgram) const {
	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".dir", dir);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1b(objectName + ".enabled", enabled);
}
void DirLight::setDir(glm::vec3 value) { dir = value; }
void DirLight::setColor(glm::vec3 value) { color = value; }

glm::vec3 DirLight::getDir() const { return dir; }
glm::vec3 DirLight::getColor() const { return color; }

//PointLight
PointLight::PointLight(glm::vec3 pos, glm::vec3 color, bool enabled)
	: pos(pos), color(color), enabled(enabled) {
}

void PointLight::set(const std::string& objectName, const Shader& shaderProgram) const {
	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".pos", pos);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1f(objectName + ".effectiveRadius", effectiveRadius);
	shaderProgram.set1b(objectName + ".enabled", enabled);
}
void PointLight::setPos(glm::vec3 value) { pos = value; }
void PointLight::setColor(glm::vec3 value) {
	effectiveRadius = calcLightRadius(color);
	color = value;
}

glm::vec3 PointLight::getPos() const { return pos; }
glm::vec3 PointLight::getColor() const { return color; }

//SpotLight
SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, bool enabled)
	: pos(pos), dir(dir), color(color), cutOff(cutOff), outerCutOff(outerCutOff), enabled(enabled) {
}

void SpotLight::set(const std::string& objectName, const Shader& shaderProgram) const {
	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".pos", pos);
	shaderProgram.setVec3(objectName + ".dir", dir);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1f(objectName + ".cutOff", cutOff);
	shaderProgram.set1f(objectName + ".outerCutOff", outerCutOff);
	shaderProgram.set1f(objectName + ".effectiveRadius", effectiveRadius);
	shaderProgram.set1b(objectName + ".enabled", enabled);
}
void SpotLight::setDir(glm::vec3 value) { dir = value; }
void SpotLight::setPos(glm::vec3 value) { pos = value; }
void SpotLight::setColor(glm::vec3 value) {
	effectiveRadius = calcLightRadius(color);
	color = value;
}

glm::vec3 SpotLight::getDir() const { return dir; }
glm::vec3 SpotLight::getPos() const { return pos; }
glm::vec3 SpotLight::getColor() const { return color; }