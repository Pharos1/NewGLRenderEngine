#version 450
out vec4 fragOut;

in vec3 worldPos;
in vec3 vertNormal;
in vec2 texCoord;
in mat3 TBN;

layout (std140, binding = 0) uniform Matrices {
	mat4 proj;
	mat4 view;
};

uniform vec3 viewPos;
uniform float specularExponent;

struct DirLight{
	bool enabled;
	vec3 dir;
	vec3 color;
};
struct PointLight{
	bool enabled;
	vec3 pos;
	vec3 color;
	float effectiveRadius;
};
struct SpotLight{
	bool enabled;
	vec3 pos;
	vec3 dir;
	vec3 color;
	float cutOff;
	float outerCutOff;
	float effectiveRadius;
};

uniform DirLight dirLight;
#define NUMBER_OF_POINT_LIGHTS 4
uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS];
#define NUMBER_OF_SPOT_LIGHTS 1
uniform SpotLight spotLights[NUMBER_OF_SPOT_LIGHTS];

layout(binding = 0) uniform sampler2D albedoTex;
layout(binding = 1) uniform sampler2D metallicTex;
layout(binding = 2) uniform sampler2D roughnessTex;
layout(binding = 3) uniform sampler2D normalTex;

//CSM
layout(std140, binding = 1) uniform LightSpaceMatrices {
	mat4 lightSpaceMatrices[16];
};
layout(binding = 4) uniform sampler2DArray shadowMap;
uniform bool csmEnabled;
uniform int cascadeCount;
uniform float farPlane;
uniform float cascadePlaneDistances[16];
uniform bool cascadeDebugView;
uniform bool freezeCSM;
uniform vec4 oldThirdViewRow;

//layout(binding = 0) uniform sampler2D gPositionBuffer;
//layout(binding = 1) uniform sampler2D gNormalBuffer;
//layout(binding = 2) uniform sampler2D gAlbedoBuffer;

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float specularExponent, float metallicValue);
float attenuation(float dist);
vec3 getNormalFromMap();

vec3 fresnelSchlick(float cosTheta, vec3 baseReflectivity);
float distributionGGX(vec3 normalVec, vec3 halfwayVec, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 normalVec, vec3 viewDir, vec3 lightDir, float roughness);

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity);
float calcShadow(vec3 normal, vec3 lightDir);

void main(){
	vec4 sampledAlbedo = texture(albedoTex, texCoord);
	if (sampledAlbedo.a < .05f){ discard; return; } //Some drivers don't return on discard

	vec3 albedo = pow(sampledAlbedo.rgb, vec3(2.2f));
	float sampledMetallic = texture(metallicTex, texCoord).b;
	float sampledRoughness = texture(roughnessTex, texCoord).b;
	vec3 sampledNormal = texture(normalTex, texCoord).xyz;

	vec3 fragNormal;
	if(sampledNormal != vec3(0.f) && TBN != mat3(0.f)){
		fragNormal = normalize(TBN * (sampledNormal * 2.f - 1.f));
	}
	else{
		fragNormal = normalize(vertNormal);
	}

	/*vec4 gPosition = texture(gPositionBuffer, texCoord);
	vec4 gNormal = texture(gNormalBuffer, texCoord);
	vec3 gAlbedo = texture(gAlbedoBuffer, texCoord).rgb;

	vec3 sampledAlbedo = pow(gAlbedo, vec3(2.2f));
	float sampledMetallic = gPosition.a;
	float sampledRoughness = gNormal.a;
	vec3 fragNormal = gNormal.rgb;
	vec3 worldPos = gPosition.rgb;
	*/

	if (cascadeDebugView) {
		//float depthValue = freezeCSM ? distance(oldViewPos, worldPos) : distance(viewPos, worldPos);
		vec4 thirdViewRow = freezeCSM ? oldThirdViewRow : vec4(view[0][2], view[1][2], view[2][2], view[3][2]);
		float depthValue = abs(dot(thirdViewRow, vec4(worldPos, 1.f)));
		//float depthValue = abs(dot(vec4(view[0][2], view[1][2], view[2][2], view[3][2]), vec4(worldPos, 1.f)));

		int layer = -1;
		for (int i = 0; i < cascadeCount; i++) {
			if (depthValue <= cascadePlaneDistances[i]) {
				layer = i;
				break;
			}
		}
		if (layer == 0) albedo = vec3(1.f, 0.f, 0.f);
		else if (layer == 1) albedo = vec3(0.f, 1.f, 0.f);
		else if (layer == 2) albedo = vec3(0.f, 0.f, 1.f);
		else albedo = vec3(1.f, 1.f, 0.f);
	}

	vec3 viewDir = normalize(viewPos - worldPos);
	
	vec3 baseReflectivity = vec3(.04f);
	baseReflectivity = mix(baseReflectivity, albedo, sampledMetallic);

	vec3 result = vec3(0.f);
	result += calcDirLight(dirLight, fragNormal, viewDir, worldPos, albedo, sampledMetallic, sampledRoughness, baseReflectivity);
	for(int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++) {
		result += calcPointLight(pointLights[i], fragNormal, viewDir, worldPos, albedo, sampledMetallic, sampledRoughness, baseReflectivity);
	}
	for(int i = 0; i < NUMBER_OF_SPOT_LIGHTS; i++) {
		result += calcSpotLight(spotLights[i], fragNormal, viewDir, worldPos, albedo, sampledMetallic, sampledRoughness, baseReflectivity);
	}

	float ambientCoeficient = .03f;
	result += ambientCoeficient * albedo;

	fragOut = vec4(result, 1.f);
}

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float specularExponent, float metallicValue){
	vec3 halfwayVec = normalize(lightDir + viewDir);

	return metallicValue * pow(max(dot(normal, halfwayVec), 0.f), specularExponent);
}
#define lightRadius 1.f
float attenuation(float dist){
	//Point Light Attenuation Without Singularity from Cem Yukse, University of Utah(http://www.cemyuksel.com/research/pointlightattenuation/pointlightattenuation.pdf)
	float radiusSquared = (lightRadius * lightRadius);
	float attenuation = 2.f / radiusSquared;
	attenuation *= (1.f - dist/sqrt((dist*dist) + radiusSquared));
	return attenuation;
}
vec3 getNormalFromMap(){
	vec3 tangentNormal = texture(normalTex, texCoord).xyz * 2.f - 1.f;
	return normalize(TBN * tangentNormal);
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
	
	float numerator = area2;
	float denominator = (NdotH2 * (area2 - 1.f) + 1.f);
	denominator = PI * denominator * denominator;
	
	return numerator / denominator;
}
float geometrySchlickGGX(float NdotV, float roughness){
	float r = (roughness + 1.f);
	float k = (r*r) / 8.f;

	float numerator = NdotV;
	float denominator  = NdotV * (1.f - k) + k;
	
	return numerator / denominator;
}
float geometrySmith(vec3 normalVec, vec3 viewDir, vec3 lightDir, float roughness){
	float NdotV = max(dot(normalVec, viewDir), 0.1);
	float NdotL = max(dot(normalVec, lightDir), 0.1);
	float ggx2  = geometrySchlickGGX(NdotV, roughness);
	float ggx1  = geometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity){
	if(light.color == vec3(0.f) || !light.enabled) return vec3(0.f);
	
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

	vec3 magicAmbient = albedo * .20f;
	return (kD * albedo / PI + specular) * radiance * NdotL * (1.f-calcShadow(normal, lightDir)) + magicAmbient;
}
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity){
	if (light.color == vec3(0.f) || !light.enabled) return vec3(0.f);
	
	vec3 lightDir = normalize(light.pos - worldPos);
	vec3 halfwayVec = normalize(viewDir + lightDir);
  
	float dist = length(light.pos - worldPos);
	float attenuation = attenuation(dist);
	vec3 radiance = light.color * attenuation;
	float NdotL = max(dot(normal, lightDir), 0.f);
	
	vec3 fresnel = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.f), baseReflectivity);

	//Cook-tolerance BRDF
	float NDF = distributionGGX(normal, halfwayVec, roughness);
	float G   = geometrySmith(normal, viewDir, lightDir, roughness);

	vec3 numerator    = NDF * G * fresnel;
	float denominator = 4.f * max(dot(normal, viewDir), 0.f) * NdotL + .0001f;
	vec3 specular     = numerator / denominator; 

	vec3 kS = fresnel;
	vec3 kD = vec3(1.f) - kS;

	kD *= 1.f - metallic;

	vec3 magicAmbient =  5*albedo * .05f / dist;
	return (kD * albedo / PI + specular) * radiance * NdotL + magicAmbient;
}
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 worldPos, vec3 albedo, float metallic, float roughness, vec3 baseReflectivity){
	if(light.color == vec3(0.f) || !light.enabled) return vec3(0.f);
	
	vec3 lightDir = normalize(light.pos - worldPos);
	vec3 halfwayVec = normalize(viewDir + lightDir);
  
	float dist = length(light.pos - worldPos);
	float attenuation = 1.f / (dist * dist);
	vec3 radiance = light.color * attenuation;
	float NdotL = max(dot(normal, lightDir), 0.f);
	
	vec3 fresnel = fresnelSchlick(max(dot(halfwayVec, viewDir), 0.f), baseReflectivity);

	//Cook-tolerance BRDF
	float NDF = distributionGGX(normal, halfwayVec, roughness);
	float G   = geometrySmith(normal, viewDir, lightDir, roughness);

	vec3 numerator = NDF * G * fresnel;
	float denominator = 4.f * max(dot(normal, viewDir), 0.f) * NdotL + .0001f;
	vec3 specular = numerator / denominator; 

	vec3 kS = fresnel;
	vec3 kD = vec3(1.f) - kS;

	kD *= 1.f - metallic;

	float theta = dot(lightDir, normalize(-light.dir)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

	return (kD * albedo / PI + specular) * radiance * NdotL * intensity;
}
float calcShadow(vec3 normal, vec3 lightDir) {
	if (!csmEnabled) return 0.f;

	//Select cascade layer
	vec4 thirdViewRow = freezeCSM ? oldThirdViewRow : vec4(view[0][2], view[1][2], view[2][2], view[3][2]);
	float depthValue = abs(dot(thirdViewRow, vec4(worldPos, 1.f)));

	int layer = -1;
	for (int i = 0; i < cascadeCount; i++) {
		if (depthValue <= cascadePlaneDistances[i]) {
			layer = i;
			break;
		}
	}
	
	vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(worldPos, 1.f);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * .5f + .5f;
	
	//Keep the shadow at 0.f when outside the far plane region of the light's frustum.
	if(projCoords.z > 1.f) {
		return 0.f;
	}

	float maxBias = .05f;
	float minBias = .005f;
	float bias = max(maxBias * (1.f - dot(normal, lightDir)), minBias);
	const float biasModifier = 6.f;//0.5f;

	bias *= 1.f / (cascadePlaneDistances[layer] * biasModifier);

	//PCF
	float shadow = 0.f;
	vec2 texelSize = 1.f / vec2(textureSize(shadowMap, 0));
	for(int x = -1; x <= 1; x++) {
		for(int y = -1; y <= 1; y++) {
			float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
			shadow += (projCoords.z - bias) > pcfDepth ? 1.f : 0.f;
		}
	}
	shadow /= 9.f;

	return shadow;
}