#version 450

in vec2 texCoord;

uniform sampler2D albedoTex;

void main(){
	if(texture(albedoTex, texCoord).a < .05f) discard;
}