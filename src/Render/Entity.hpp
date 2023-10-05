#include "../pch.h"
#include "Model.hpp"

class Transform {
private:
	glm::vec3 localPos = glm::vec3(0.f);
	glm::vec3 localRot = glm::vec3(0.f);
	glm::vec3 localScale = glm::vec3(1.f);

	//Global space
	glm::mat4 modelMat = glm::mat4(1.f);

	bool changed = true;

public:
	glm::mat4 getLocalModelMat();

	void calcModelMatrix(const glm::mat4& parentGlobalModelMatrix = glm::mat4(1.f));

	void setLocalPos(const glm::vec3 value);
	void setLocalRot(const glm::vec3 value);
	void setLocalScale(const glm::vec3 value);

	glm::vec3 getLocalPos() const;
	glm::vec3 getLocalRot() const;
	glm::vec3 getLocalScale() const;
	glm::mat4 getGlobalModelMat() const;

	bool isChanged() const;
};

class Entity : public Model {
public:
	Transform transform;
	std::vector<Entity*> children; //Note: could also make the entity own the vectors on the stack.
	Entity* parent = nullptr;
	int verticesCount = 0;

	Entity() = default;

	void addChild(Entity& child);
	void updateSelf();
	void updateSelfAndChild();
	void draw(const Shader& shader);
	void loadModel(const std::string& path);
	void updateVertCount();
};