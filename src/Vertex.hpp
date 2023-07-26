#pragma once
#include <GLM/glm.hpp>
#include <ostream>

class Vertex {
public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoord);
	Vertex();
};
std::ostream& operator<<(std::ostream& os, const glm::vec4& other);
std::ostream& operator<<(std::ostream& os, const glm::vec3& other);
std::ostream& operator<<(std::ostream& os, const glm::vec2& other);