#include "pch.h"
#include "Camera.hpp"

#include "DeltaTime.hpp"

Camera::Camera(glm::vec3 pos, float speed, float mouseSensitivity)
	: pos(pos), speed(speed), mouseSensitivity(mouseSensitivity){
}

const glm::mat4& Camera::getView() const {
	return view;
}
void Camera::updateView() {
	view = glm::lookAt(pos, pos + front, up);
}
void Camera::processInput(GLFWwindow* window) {
	float speedAmplifier = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? 3.f : (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? .15f : 1.f);

	if (glfwGetKey(window, GLFW_KEY_W))
		pos += speed * front * DT::time * speedAmplifier;
	if (glfwGetKey(window, GLFW_KEY_S))
		pos -= speed * front * DT::time * speedAmplifier;
	if (glfwGetKey(window, GLFW_KEY_A))
		pos -= glm::normalize(glm::cross(front, up)) * speed * DT::time * speedAmplifier;
	if (glfwGetKey(window, GLFW_KEY_D))
		pos = pos + glm::normalize(glm::cross(front, up)) * speed * DT::time * speedAmplifier;

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
	if (pitch <= -90.f) pitch = -89.9f;

	glm::vec3 dir;
	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(dir);

	updateView();
}