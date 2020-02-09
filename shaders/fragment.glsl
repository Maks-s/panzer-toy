#version 440 core
out vec4 FragColor;

in vec2 texCoords;
uniform float time;
uniform sampler2D textureDiffuse0;

const float PI = 3.14159265359;

void main() {
	FragColor = texture(textureDiffuse0, texCoords);
	// FragColor = FragColor * vec4(1f, sin(mod(time, 1) * PI), .2f, 1f);
}
