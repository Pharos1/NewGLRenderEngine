#version 450
out vec4 fragOut;

in vec3 worldPos;

in vec3 normal;
in vec2 texCoord;
in vec3 color;

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

layout(binding = 0) uniform sampler2D texture0;

float spec(vec3 lightDir, vec3 normal, vec3 viewDir, float specularExponent){
	vec3 halfwayDir = normalize(lightDir + viewDir);

	return pow(max(dot(normal, halfwayDir), 0.f), specularExponent);
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
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float specularExponent){
	float diff = max(dot(-light.dir, normal), 0.f);
	float spec = spec(-light.dir, normal, viewDir, specularExponent);

	return (diff + spec) * light.color;
}
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float specularExponent, vec3 worldPos){
	vec3 lightDir = normalize(light.pos - worldPos);
	float diff = max(dot(lightDir, normal), 0.f);
	float spec = spec(lightDir, normal, viewDir, specularExponent);

	float dist = distance(light.pos, worldPos);
	float attenuation = attenuation(distance(light.pos, worldPos));

	return (diff + spec) * light.color * attenuation;
}
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, float specularExponent, vec3 worldPos){
	vec3 lightDir = normalize(light.pos - worldPos);
	float diff = max(dot(lightDir, normal), 0.f);
	float spec = spec(lightDir, normal, viewDir, specularExponent);

	//float dist = distance(light.pos, worldPos);
	float attenuation = attenuation(distance(light.pos, worldPos));

	//Spotlight range
	float theta = dot(lightDir, normalize(-light.dir)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.f, 1.f);

	return light.color * (diff + spec) * attenuation * intensity;
}
void main(){
	vec3 viewDir = normalize(viewPos - worldPos);
	vec3 result = calcDirLight(dirLight, normal, viewDir, specularExponent);
	result += calcPointLight(pointLight, normal, viewDir, specularExponent, worldPos);
	result += calcSpotLight(spotLight, normal, viewDir, specularExponent, worldPos);

	vec3 ambient = vec3(0.1f);
	result += ambient;
	fragOut = vec4(texture(texture0, texCoord).rgb * result, 1.f);
}