#version 450
//in vec2 texCoord;

//Note: texture fetches are too expensive! //layout(binding = 0) uniform sampler2D albedoTex;

void main() {
	//Note: texture fetches are too expensive! //if (texture(albedoTex, texCoord).a < .05f){ discard; return; } //Some drivers don't return on discard
}