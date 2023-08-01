#pragma once
#include "pch.h"
#include "DeltaTime.hpp"

struct Camera {
	glm::mat4 view{1};
	float pitch = -20.f;
	float yaw = -90.f;
	bool firstMouse = true;
	double lastX{}, lastY{};

	//Optional
	float speed = 75.f;
	float mouseSensitivity = 0.1f;
	glm::vec3 pos{};
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	Camera(glm::vec3 pos, float speed = 7.5f, float mouseSensitivity = .1f);
	Camera() = default;

	const glm::mat4& getView() const;
	void updateView();
	void processInput(GLFWwindow* window);
	void processMouse(double xPos, double yPos);
	void calcFrontVec();
};