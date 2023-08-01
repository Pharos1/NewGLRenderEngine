#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

out vec3 worldPos;

out vec2 texCoord;
out vec3 vertNormal;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
	worldPos = (model * vec4(inPos, 1.f)).xyz;
	gl_Position = proj * view * vec4(worldPos, 1.f);
	
	texCoord = inTexCoord;

	mat3 normalMatrix = transpose(inverse(mat3(model))); //Transpose is really expensive
	vertNormal = normalize(normalMatrix * inNormal);

	if(inTangent == vec3(0.f))
		TBN = mat3(0.f);
	else{
		vec3 T = normalize(normalMatrix * inTangent);
		
		T = normalize(T - dot(T, vertNormal) * vertNormal);
		vec3 B = cross(vertNormal, T); //use bitangent inside model.cpp
		
		TBN = mat3(T, B, vertNormal);
	}
}