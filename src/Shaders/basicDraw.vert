#version 450
layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
	texCoord = inTexCoord;//(matrix * vec3(inTexCoord, 0.f)).xy;
	gl_Position = proj * view * model * vec4(inPos, 1.f);
}