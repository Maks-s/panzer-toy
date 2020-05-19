#version 440 core
out vec4 FragColor;

in vec2 texCoords;

uniform vec3 color;
uniform sampler2D alphaTexture;

void main() {
	float alpha = texture(alphaTexture, texCoords).r;
	FragColor = vec4(color, alpha);
}
