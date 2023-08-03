#version 450
out vec4 fragOut;

in vec2 texCoord;

void main(){
	vec3 result = vec3(1.f);
	result *= pow(texCoord.y, 2.2);
	fragOut = vec4(result, 1.f);
}