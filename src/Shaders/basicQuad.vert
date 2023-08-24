#version 450
layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTexCoord;

out vec2 texCoord;

void main(){
	texCoord = inTexCoord;
	gl_Position = vec4(inPos.xy, 1.f, 1.f);
}