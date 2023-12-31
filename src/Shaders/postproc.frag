#version 450
out vec4 fragOut;

in vec2 texCoord;

layout(binding = 0) uniform sampler2D colorBuffer;
layout(binding = 1) uniform sampler2D bloomBlur;

uniform float exposure;
uniform float gamma;
uniform bool gammaOn;

uniform int tonemapMode;

uniform float maxRadiance;

//All credits Stephen Hill (@self_shadow) for inventing the ACES Tone Mapping
mat3 ACESInputMat = { //The matrix is a transposed version of the original so it matches the row major order of glsl
	vec3(0.59719f, 0.07600f, 0.02840f),
	vec3(0.35458f, 0.90834f, 0.13383f),
	vec3(0.04823f, 0.01566f, 0.83777f),
};
mat3 ACESOutputMat = { //The matrix is a transposed version of the original so it matches the column major order of glsl
	vec3( 1.60475f, -0.10208f, -0.00327f),
	vec3(-0.53108f,  1.10813f, -0.07276f),
	vec3(-0.07367f, -0.00605f,  1.07602f),
};
vec3 RRTAndODTFit(vec3 v){
	vec3 a = v * (v + 0.0245786f) - 0.000090537f;
	vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	return a / b;
}
vec3 hillACES(vec3 color){
	color = ACESInputMat * color;

	// Apply RRT and ODT
	color = RRTAndODTFit(color);

	color = ACESOutputMat * color;
	return color;
}

float luminance(vec3 color){
	return dot(color, vec3(.2126f, .7152f, .0722f));
}
vec3 changeLuminance(vec3 color, float lumOut){
	float lumIn = luminance(color);
	return color * (lumOut / lumIn);
}
vec3 reinhard(vec3 color){
	float lumOld = luminance(color);
	float lumNew = lumOld / (1 + lumOld);

	return changeLuminance(color, lumNew);
}
vec3 reinhardExtended(vec3 color){
	float lumOld = luminance(color);

	float numerator = lumOld * (1 + lumOld / pow(maxRadiance, 2));
	float lumNew = numerator / (1 + lumOld);

	return changeLuminance(color, lumNew);
}

#define A 0.15
#define B 0.50
#define C 0.10
#define D 0.20
#define E 0.02
#define F 0.30

//Uncharted2 tonemapping is from, as you might have guessed, Uncharted2
vec3 uncharted2Equation(vec3 x){
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
vec3 uncharted2(vec3 color){
	float exposure_bias = 2.0f;
	vec3 curr = uncharted2Equation(color * exposure_bias);
	
	vec3 W = vec3(11.2f);
	vec3 white_scale = vec3(1.0f) / uncharted2Equation(W);
	return curr * white_scale;
}
vec3 manualExposure(vec3 color){
	// exposure tone mapping
	vec3 result = vec3(1.0) - exp(-color * exposure);
  
	return vec4(result, 1.0);
}
#define a 2.51f
#define b 0.03f
#define c 2.43f
#define d 0.59f
#define e 0.14f

//Credit to Krzysztof Narkowicz(https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/) for his ACES approximation
vec3 narkowiczACES(vec3 color){
	return (color*(a*color+b))/(color*(c*color+d)+e);
}

void main(){
	vec3 result = texture(colorBuffer, texCoord).rgb;

	if(tonemapMode == 1) result = reinhard(result);
	else if(tonemapMode == 2) result = reinhardExtended(result);
	else if(tonemapMode == 3) result = uncharted2(result);
	else if(tonemapMode == 4) result = hillACES(result);
	else if(tonemapMode == 5) result = narkowiczACES(result);
	else if(tonemapMode == 6) result = manualExposure(result);

	if(gammaOn){
		result = pow(result, 1.f / vec3(gamma));
	}
	fragOut = vec4(result, 1.f);
}