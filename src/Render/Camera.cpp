#include "../pch.h"
#include "Camera.hpp"

#include "../Utilities/Time.hpp"

Camera::Camera(glm::vec3 pos, float speed, float mouseSensitivity)
	: pos(pos), speed(speed), mouseSensitivity(mouseSensitivity){
	calcFrontVec();
}

const glm::mat4& Camera::getView() const {
	return view;
}
void Camera::updateView() {
	view = glm::lookAt(pos, pos + front, up);
}
void Camera::processInput(GLFWwindow* window) {
	float speedAmplifier = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? 3.f : (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? .15f : 1.f);
	float finalSpeed = Time::deltaTime * speedAmplifier * speed;

	if (glfwGetKey(window, GLFW_KEY_W))
		pos += front * finalSpeed;
	if (glfwGetKey(window, GLFW_KEY_S))
		pos -= front * finalSpeed;
	if (glfwGetKey(window, GLFW_KEY_A))
		pos -= glm::normalize(glm::cross(front, up)) * finalSpeed;
	if (glfwGetKey(window, GLFW_KEY_D))
		pos += glm::normalize(glm::cross(front, up)) * finalSpeed;

	updateView();
}
void Camera::processMouse(double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch >= 90.f) pitch = 89.9f;
	else if (pitch <= -90.f) pitch = -89.9f;
	if (yaw >= 360.f) yaw -= 360.f;
	else if (yaw <= -360.f) yaw += 360.f;

	calcFrontVec();
	updateView();
}
void Camera::calcFrontVec() {
	glm::vec3 dir;
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(dir);
}