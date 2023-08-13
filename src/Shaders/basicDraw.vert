#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec3 worldPos;
out vec2 texCoord;
out vec3 vertNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
	worldPos = vec3(model * vec4(inPos, 1.f));
	gl_Position = proj * view * vec4(worldPos, 1.f);
	texCoord = inTexCoord;
	vertNormal = transpose(inverse(mat3(model))) * inNormal;
}