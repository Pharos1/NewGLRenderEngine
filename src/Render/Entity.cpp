#include "../pch.h"
#include "Entity.hpp"
#include "../Utilities/Time.hpp"

//Transform
glm::mat4 Transform::getLocalModelMat() {
	glm::mat4 transformX = glm::rotate(glm::mat4(1.f), glm::radians(localRot.x), glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 transformY = glm::rotate(glm::mat4(1.f), glm::radians(localRot.y), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 transformZ = glm::rotate(glm::mat4(1.f), glm::radians(localRot.z), glm::vec3(0.f, 0.f, 1.f));

	glm::mat4 roationMatrix = transformX * transformY * transformZ;

	return glm::translate(glm::mat4(1.f), localPos) * roationMatrix * glm::scale(glm::mat4(1.f), localScale);
}

void Transform::calcModelMatrix(const glm::mat4& parentGlobalModelMatrix) {
	modelMat = parentGlobalModelMatrix * getLocalModelMat();
	changed = false;
}

void Transform::setLocalPos(const glm::vec3 value) {
	localPos = value;
	changed = true;
}
void Transform::setLocalRot(const glm::vec3 value) {
	localRot = value;
	changed = true;
}
void Transform::setLocalScale(const glm::vec3 value) {
	localScale = value;
	changed = true;
}

glm::vec3 Transform::getLocalPos() const { return localPos; }
glm::vec3 Transform::getLocalRot() const { return localRot; }
glm::vec3 Transform::getLocalScale() const { return localScale; }
glm::mat4 Transform::getGlobalModelMat() const { return modelMat; }

bool Transform::isChanged() const { return changed; }

//Entity
void Entity::addChild(Entity& child) {
	children.push_back(&child);
	child.parent = this;
	updateVertCount();
}
void Entity::updateSelf() {
	if (transform.isChanged()) {
		if (parent)
			transform.calcModelMatrix(parent->transform.getGlobalModelMat());
		else
			transform.calcModelMatrix();
	}
}
void Entity::updateSelfAndChild() {
	updateSelf();

	for (auto& child : children) {
		child->updateSelfAndChild();
	}
}
void Entity::draw(const Shader& shader) {
	updateSelf();

	shader.use();
	shader.setMat4("model", transform.getGlobalModelMat());

	Model::draw();

	for (auto& child : children) {
		child->draw(shader);
	}
}
void Entity::loadModel(const std::string& path) {
	Model::loadModel(path);
	updateVertCount();
}
void Entity::updateVertCount() {
	for (auto& mesh : meshes) {
		verticesCount += mesh.verticesCount;
	}
	for (auto child : children) {
		verticesCount += child->verticesCount;
	}
}