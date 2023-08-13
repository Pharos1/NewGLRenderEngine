#version 450
layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTexCoord;

out vec2 texCoord;
//uniform mat4 view;

void main(){
	//mat3 matrix = mat3(view);
	texCoord = inTexCoord;//(matrix * vec3(inTexCoord, 0.f)).xy;
	gl_Position = vec4(inPos.xy, 1.f, 1.f);
}