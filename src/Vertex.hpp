#pragma once
#include <GLM/glm.hpp>
#include <ostream>

class Vertex {
public:
	glm::vec3 pos{0};
	glm::vec3 normal{0};
	glm::vec2 texCoord{0};
	glm::vec3 tangent{0};

	Vertex(glm::vec3 pos, glm::vec3 normal = glm::vec3(0.f), glm::vec2 texCoord = glm::vec2(0.f), glm::vec3 tangent = glm::vec3(0.f));
	Vertex() = default;

	static void setAttribArrays();
};
std::ostream& operator<<(std::ostream& os, const glm::vec4& other);
std::ostream& operator<<(std::ostream& os, const glm::vec3& other);
std::ostream& operator<<(std::ostream& os, const glm::vec2& other);