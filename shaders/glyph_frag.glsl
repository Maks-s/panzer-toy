#version 440 core
out vec4 FragColor;

in vec2 texCoords;

uniform vec3 color;
uniform sampler2D textureText;

void main() {
	float alpha = texture(textureText, texCoords).r;
	FragColor = vec4(color, alpha);
}
