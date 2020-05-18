#version 440 core
out vec4 FragColor;

in vec2 texCoords;
// @TODO: Remove time
uniform float time;
uniform sampler2D textureDiffuse0;

void main() {
	FragColor = texture(textureDiffuse0, texCoords);
}
