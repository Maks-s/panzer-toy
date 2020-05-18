#version 440 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D textureDiffuse0;

void main() {
	FragColor = texture(textureDiffuse0, texCoords);
}
