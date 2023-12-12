#pragma once
#include "../pch.h"
#include "../Utilities/Time.hpp"

class Camera {
private:
	bool firstMouse = true;

public:
	//Projection
	float farPlane = 250.f;
	float nearPlane = .1f;
	float fov = 45.f;

	glm::mat4 view;
	glm::mat4 proj;

	//Transformation
	double lastX{}, lastY{};
	float pitch = -20.f;
	float yaw = -90.f;
	glm::vec3 pos{};
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	//Settings
	float speed = 75.f;
	float mouseSensitivity = 0.1f;

	Camera(glm::vec3 pos, float speed = 7.5f, float mouseSensitivity = .1f);
	Camera() = default;

	void updateView();
	void updateProj(float aspectRatio);
	void processInput(GLFWwindow* window);
	void processMouse(double xPos, double yPos);
	void calcFrontVec();
};