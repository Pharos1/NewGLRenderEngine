#version 450
layout(location = 0) in vec3 inPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 lightPos;

void main(){
	gl_Position = proj * view * (model * vec4(inPos, 1.f) + vec4(lightPos, 0.f));
}