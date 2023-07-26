#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec3 worldPos;

out vec3 normal;
out vec2 texCoord;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
	worldPos = (model * vec4(inPos, 1.f)).xyz;
	gl_Position = proj * view * vec4(worldPos, 1.f);
	
	color = ((inNormal+1.f)/2.f);

	mat3 normalMatrix = transpose(inverse(mat3(model))); //Transpose is really expensive
	normal = normalize(normalMatrix * inNormal);
	texCoord = inTexCoord;
}