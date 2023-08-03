#version 450
out vec4 fragOut;

in vec2 texCoord;

layout(binding = 0) uniform sampler2D image;

void main(){
	fragOut = texture(image, texCoord);
}