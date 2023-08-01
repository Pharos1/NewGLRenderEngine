#include "pch.h"
#include "Light.hpp"

//DirLight
DirLight::DirLight(glm::vec3 dir, glm::vec3 color)
	: dir(dir), color(color) {
}
void DirLight::set(const std::string& objectName, Shader& shaderProgram) const {
	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".dir", dir);
	shaderProgram.setVec3(objectName + ".color", color);
}

//PointLight
PointLight::PointLight(glm::vec3 pos, glm::vec3 color)
	: pos(pos), color(color) {
}
void PointLight::set(const std::string& objectName, Shader& shaderProgram) const {
	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".pos", pos);
	shaderProgram.setVec3(objectName + ".color", color);
}

//SpotLight
SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff)
	: pos(pos), dir(dir), color(color), cutOff(cutOff), outerCutOff(outerCutOff) {
}
void SpotLight::set(const std::string& objectName, Shader& shaderProgram) const {
	shaderProgram.use();
	shaderProgram.setVec3(objectName + ".pos", pos);
	shaderProgram.setVec3(objectName + ".dir", dir);
	shaderProgram.setVec3(objectName + ".color", color);
	shaderProgram.set1f(objectName + ".cutOff", cutOff);
	shaderProgram.set1f(objectName + ".outerCutOff", outerCutOff);
}