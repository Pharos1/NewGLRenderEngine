#version 450
layout (location = 0) out vec4 gPosition; //Position + Metallic
layout (location = 1) out vec4 gNormal; //Normal + Roughness
layout (location = 2) out vec3 gAlbedo;

in vec3 worldPos;
in vec2 texCoord;
in vec3 vertNormal;
in mat3 TBN;

layout(binding = 0) uniform sampler2D albedoTex;
layout(binding = 1) uniform sampler2D metallicTex;
layout(binding = 2) uniform sampler2D roughnessTex;
layout(binding = 3) uniform sampler2D normalTex;

vec3 getNormalFromMap(){
	vec3 tangentNormal = texture(normalTex, texCoord).xyz * 2.f - 1.f;
	return normalize(TBN * tangentNormal);
}
void main(){
	vec4 sampledAlbedo = texture(albedoTex, texCoord);
	if(sampledAlbedo.a < .05f) discard;

	float sampledMetallic = texture(metallicTex, texCoord).r;
	float sampledRoughness = texture(roughnessTex, texCoord).r;
	vec3 sampledNormal = texture(normalTex, texCoord).rgb;

	vec3 fragNormal;
	if(sampledNormal != vec3(0.f) && TBN != mat3(0.f)){
		fragNormal = getNormalFromMap();
	}
	else{
		fragNormal = normalize(vertNormal);
	}

	gPosition = vec4(worldPos, sampledMetallic);
	gNormal = vec4(fragNormal, sampledRoughness);
	gAlbedo = vec3(sampledAlbedo);
}