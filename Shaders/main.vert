#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inTangent;

out vec3 worldPos;

out vec3 normal;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
	worldPos = (model * vec4(inPos, 1.f)).xyz;
	gl_Position = proj * view * vec4(worldPos, 1.f);
	
	texCoord = inTexCoord;

	mat3 normalMatrix = transpose(inverse(mat3(model))); //Transpose is really expensive
	normal = normalize(normalMatrix * inNormal);

	if(inTangent == vec3(0.f))
		TBN = mat3(0.f);
	else{
		vec3 T = normalize(normalMatrix * inTangent);
		
		T = normalize(T - dot(T, normal) * normal);
		vec3 B = cross(normal, T);
		
		TBN = mat3(T, B, normal);
	}
}