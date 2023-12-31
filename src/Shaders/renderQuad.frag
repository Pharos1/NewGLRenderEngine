#version 450
out vec4 fragOut;

in vec2 texCoord;

//layout(binding = 0) uniform sampler2D image;
layout(binding = 0) uniform sampler2DArray image;

uniform int layer;

void main(){
	//fragOut = texture(image, texCoord);
	fragOut = vec4(vec3(texture(image, vec3(texCoord, layer)).r), 1.f);
}