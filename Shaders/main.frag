#version 450
out vec4 fragOut;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;
in mat3 TBN;

uniform vec3 viewPos;
uniform float specularExponent;

struct DirLight{
	vec3 dir;
	vec3 color;
};
struct PointLight{
	vec3 pos;
	vec3 color;
};
struct SpotLight{
	vec3 pos;
	vec3 dir;
	vec3 color;
	float cutOff;
	float outerCutOff;
};
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;


layout(binding = 0) uniform sampler2D albedoTex;
layout(binding = 1) uniform sampler2D metallicTex;
layout(binding = 2) uniform sampler2D roughnessTex;
layout(binding = 3) uniform sampler2D normalTex;

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float specularExponent, float metallicValue){
	vec3 halfwayDir = normalize(lightDir + viewDir);

	return metallicValue * pow(max(dot(normal, halfwayDir), 0.f), specularExponent);
}
#define lightRadius 1.15f
float attenuation(float dist){
	//Point Light Attenuation Without Singularity from Cem Yukse, University of Utah(http://www.cemyuksel.com/research/pointlightattenuation/pointlightattenuation.pdf)
	//Radius constant is 1.15.
	float radiusSquared = (lightRadius * lightRadius);
	float attenuation = 2.f / radiusSquared;
	attenuation *= (1.f - dist/sqrt((dist*dist) + radiusSquared));
	return attenuation;
}
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float specularExponent, vec3 albedoColor, float metallicValue){
	if(light.color == vec3(0.f)) return vec3(0.f);
	vec3 lightDir = normalize(-light.dir);
	float diff = max(dot(lightDir, normal), 0.f);
	float spec = .3f * spec(lightDir, normal, viewDir, specularExponent, metallicValue);

	return (diff * albedoColor + spec) * light.color;
}
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float specularExponent, vec3 albedoColor, float metallicValue, vec3 worldPos){
	if(light.color == vec3(0.f)) return vec3(0.f);
	vec3 lightDir = normalize(light.pos - worldPos);
	float diff = max(dot(lightDir, normal), 0.f);
	float spec = spec(lightDir, normal, viewDir, specularExponent, metallicValue);

	float dist = distance(light.pos, worldPos);
	float attenuation = attenuation(distance(light.pos, worldPos));

	return (diff * albedoColor + spec) * light.color * attenuation;
}
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, float specularExponent, vec3 albedoColor, float metallicValue, vec3 worldPos){
	if(light.color == vec3(0.f)) return vec3(0.f);
	vec3 lightDir = normalize(light.pos - worldPos);
	float diff = max(dot(lightDir, normal), 0.f);
	float spec = spec(lightDir, normal, viewDir, specularExponent, metallicValue);

	//float dist = distance(light.pos, worldPos);
	float attenuation = attenuation(distance(light.pos, worldPos));

	//Spotlight range
	float theta = dot(lightDir, normalize(-light.dir)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

	return (diff * albedoColor + spec) * light.color * attenuation * intensity;
}
vec3 getNormalFromMap(){
    vec3 tangentNormal = texture(normalTex, texCoord).xyz * 2.f - 1.f;
    return normalize(TBN * tangentNormal);
}

void main(){
	vec3 albedoColor = pow(texture(albedoTex, texCoord).rgb, vec3(2.2f));
	float metallicValue = texture(metallicTex, texCoord).r;

	vec3 usedNormal;
	vec3 sampledNormal = texture(normalTex, texCoord).rgb;
	if(sampledNormal != vec3(0.f) && TBN != mat3(0.f)){
		usedNormal = getNormalFromMap();
	}
	else{
		usedNormal = normalize(normal);
	}

	vec3 viewDir = normalize(viewPos - worldPos);

	vec3 result = calcDirLight(dirLight, usedNormal, viewDir, specularExponent, albedoColor, metallicValue);
	result += calcPointLight(pointLight, usedNormal, viewDir, specularExponent, albedoColor, metallicValue, worldPos);
	result += calcSpotLight(spotLight, usedNormal, viewDir, specularExponent, albedoColor, metallicValue, worldPos);

	vec3 ambient = vec3(0.05f);
	result += ambient * albedoColor;

	fragOut = vec4(result, 1.f);
}