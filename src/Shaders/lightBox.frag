#version 450
out vec4 fragOut;

uniform vec3 lightColor;

void main(){
	fragOut = vec4(lightColor, 1.f);
}