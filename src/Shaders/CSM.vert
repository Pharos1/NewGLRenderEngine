#version 450
layout (location = 0) in vec3 inPos;
layout (location = 2) in vec2 inTexCoord;
uniform mat4 model;

out vec2 vertTexCoord;

void main() {
	gl_Position = model * vec4(inPos, 1.f);
	vertTexCoord = inTexCoord;
}