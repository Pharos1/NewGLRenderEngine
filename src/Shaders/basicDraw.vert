#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout (std140, binding = 0) uniform Matrices {
	mat4 proj;
	mat4 view;
};

out vec3 worldPos;
out vec2 texCoord;
out vec3 vertNormal;

uniform mat4 model;

void main(){
	worldPos = vec3(model * vec4(inPos, 1.f));
	gl_Position = proj * view * vec4(worldPos, 1.f);
	texCoord = inTexCoord;
	vertNormal = transpose(inverse(mat3(model))) * inNormal;
}