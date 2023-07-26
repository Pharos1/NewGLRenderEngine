#include "pch.h"
#include "Vertex.hpp"

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoord)
	: pos(pos), normal(normal), texCoord(texCoord){
}
Vertex::Vertex() {};

std::ostream& operator<<(std::ostream& os, const glm::vec4& other) {
	os << other.x << ", " << other.y << ", " << other.z << ", " << other.w;
	return os;
}
std::ostream& operator<<(std::ostream& os, const glm::vec3& other) {
	os << other.x << ", " << other.y << ", " << other.z;
	return os;
}
std::ostream& operator<<(std::ostream& os, const glm::vec2& other) {
	os << other.x << ", " << other.y;
	return os;
}