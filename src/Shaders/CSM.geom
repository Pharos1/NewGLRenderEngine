#version 450
layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std140, binding = 1) uniform LightSpaceMatrices {
	mat4 lightSpaceMatrices[16]; //16 will be the maximum number of cascades
};

//in vec2 vertTexCoord[];
//out vec2 texCoord;
//
void main() {
	for (int i = 0; i < 3; i++) {
		gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		//texCoord = vertTexCoord[i];
		EmitVertex();
	}
	EndPrimitive();
}