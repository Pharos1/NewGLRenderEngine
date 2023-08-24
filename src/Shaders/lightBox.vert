#version 450
layout(location = 0) in vec3 inPos;

layout (std140, binding = 0) uniform Matrices {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

uniform vec3 lightPos;

void main(){
	gl_Position = proj * view * (model * vec4(inPos, 1.f) + vec4(lightPos, 0.f));
}