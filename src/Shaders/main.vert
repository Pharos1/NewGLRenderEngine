#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

out vec3 worldPos;

out vec2 texCoord;
out vec3 vertNormal;
out mat3 TBN;

layout (std140, binding = 0) uniform Matrices {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

void main(){
	worldPos = (model * vec4(inPos, 1.f)).xyz;
	gl_Position = proj * view * vec4(worldPos, 1.f);
	
	texCoord = inTexCoord;

	mat3 normalMatrix = transpose(inverse(mat3(model))); //Transpose is really expensive
	vertNormal = normalize(normalMatrix * inNormal);

	if(inTangent == vec3(0.f))// || inBitangent == vec3(0.f))
		TBN = mat3(0.f);
	else{
		vec3 T = normalize(normalMatrix * inTangent);
		//vec3 B = normalize(normalMatrix * inBitangent);

		T = normalize(T - dot(T, vertNormal) * vertNormal); //A little slower, but helps prevent wrong TBN matrices. Described by Joey at https://learnopengl.com/Advanced-Lighting/Normal-Mapping
		vec3 B = cross(vertNormal, T);
		
		TBN = mat3(T, B, vertNormal);
	}
}