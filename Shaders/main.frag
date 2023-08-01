#version 450
out vec4 fragOut;

in vec3 worldPos;
in vec3 vertNormal;
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

vec3 getNormalFromMap(){
	vec3 tangentNormal = texture(normalTex, texCoord).xyz * 2.f - 1.f;
	return normalize(TBN * tangentNormal);
}
float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float specularExponent, float metallicValue){
	vec3 halfwayVec = normalize(lightDir + viewDir);

	return metallicValue * pow(max(dot(normal, halfwayVec), 0.f), specularExponent);
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


vec3 fresnelSchlick(float cosTheta, vec3 baseReflectivity){
	return baseReflectivity + (1.f - baseReflectivity) * pow(clamp(1.f - cosTheta, 0.f, 1.f), 5.f);
}
#define PI 3.14159265359f
float distributionGGX(vec3 normalVec, vec3 halfwayVec, float roughness){
	float area      = roughness*roughness;
	float area2     = area*area;
	float NdotH  = max(dot(normalVec, halfwayVec), 0.f);
	float NdotH2 = NdotH*NdotH;
	
	float num   = area2;
	float denom = (NdotH2 * (area2 - 1.f) + 1.f);
	denom = PI * denom * denom;
	
	return num / denom;
}
float geometrySchlickGGX(float NdotV, float roughness){
	float r = (roughness + 1.f);
	float k = (r*r) / 8.f;

	float num = NdotV;
	float denom = NdotV * (1.f - k) + k;
	
	return num / denom;
}
float geometrySmith(vec3 normalVec, vec3 viewDir, vec3 lightDir, float roughness){
	float NdotV = max(dot(normalVec, viewDir), 0.1);
	float NdotL = max(dot(normalVec, lightDir), 0.1);
	float ggx2  = geometrySchlickGGX(NdotV, roughness);
	float ggx1  = geometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity){
	if(light.color == vec3(0.f)) return vec3(0.f);
	
	vec3 lightDir = -normalize(light.dir);
	vec3 halfwayVec = normalize(viewDir + lightDir);
  
	vec3 radiance = light.color;

	vec3 fresnel = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.f), baseReflectivity);

	//Cook-tolerance BRDF
	float NDF = distributionGGX(normal, halfwayVec, roughness);
	float G   = geometrySmith(normal, viewDir, lightDir, roughness);

	vec3 numerator    = NDF * G * fresnel;
	float denominator = 4.f * max(dot(normal, viewDir), 0.f) * max(dot(normal, lightDir), .1f)  + .0001f;
	vec3 specular     = numerator / denominator; 

	vec3 kS = fresnel;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	float NdotL = max(dot(normal, lightDir), 0.f);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity){
	if(light.color == vec3(0.f)) return vec3(0.f);
	
	vec3 lightDir = normalize(light.pos - worldPos);
	vec3 halfwayVec = normalize(viewDir + lightDir);
  
	float dist = length(light.pos - worldPos);
	float attenuation = 1.f / (dist * dist);
	vec3 radiance = light.color * attenuation;
	
	vec3 fresnel = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.f), baseReflectivity);

	//Cook-tolerance BRDF
	float NDF = distributionGGX(normal, halfwayVec, roughness);
	float G   = geometrySmith(normal, viewDir, lightDir, roughness);

	vec3 numerator    = NDF * G * fresnel;
	float denominator = 4.f * max(dot(normal, viewDir), 0.f) * max(dot(normal, lightDir), .1f)  + .0001f;
	vec3 specular     = numerator / denominator; 

	vec3 kS = fresnel;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	float NdotL = max(dot(normal, lightDir), 0.f);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity){
	if(light.color == vec3(0.f)) return vec3(0.f);
	
	vec3 lightDir = normalize(light.pos - worldPos);
	vec3 halfwayVec = normalize(viewDir + lightDir);
  
	float dist = length(light.pos - worldPos);
	float attenuation = 1.f / (dist * dist);
	vec3 radiance = light.color * attenuation;
	
	vec3 fresnel = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.f), baseReflectivity);

	//Cook-tolerance BRDF
	float NDF = distributionGGX(normal, halfwayVec, roughness);
	float G   = geometrySmith(normal, viewDir, lightDir, roughness);

	vec3 numerator    = NDF * G * fresnel;
	float denominator = 4.f * max(dot(normal, viewDir), 0.f) * max(dot(normal, lightDir), .1f)  + .0001f;
	vec3 specular     = numerator / denominator; 

	vec3 kS = fresnel;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metallic;

	float NdotL = max(dot(normal, lightDir), 0.f);

	float theta = dot(lightDir, normalize(-light.dir)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

	return (kD * albedo / PI + specular) * radiance * NdotL * intensity;
}

void main(){
	if(texture(albedoTex, texCoord).a < .05f) discard;

	vec3 sampledAlbedo = pow(texture(albedoTex, texCoord).rgb, vec3(2.2f));
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

	vec3 viewDir = normalize(viewPos - worldPos);

	//vec3 result = calcDirLight(dirLight, fragNormal, viewDir, specularExponent, sampledAlbedo, sampledMetallic);
	//result += calcPointLight(pointLight, fragNormal, viewDir, specularExponent, sampledAlbedo, sampledMetallic, worldPos);
	//result += calcSpotLight(spotLight, fragNormal, viewDir, specularExponent, sampledAlbedo, sampledMetallic, worldPos);
	
	//vec3 ambient = vec3(0.05f);
	//result += ambient * sampledAlbedo;

	
	vec3 baseReflectivity = vec3(0.04);
	baseReflectivity = mix(baseReflectivity, sampledAlbedo, sampledMetallic);

	vec3 result = vec3(0.f);
	result += calcDirLight(dirLight, fragNormal, viewDir, worldPos, sampledAlbedo, sampledMetallic, sampledRoughness, baseReflectivity);
	result += calcPointLight(pointLight, fragNormal, viewDir, worldPos, sampledAlbedo, sampledMetallic, sampledRoughness, baseReflectivity);
	result += calcSpotLight(spotLight, fragNormal, viewDir, worldPos, sampledAlbedo, sampledMetallic, sampledRoughness, baseReflectivity);

	vec3 ambient = vec3(0.03) * sampledAlbedo;
	result += ambient;

	fragOut = vec4(result, 1.f);
}