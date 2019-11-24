#version 440 core
out vec4 FragColor;

in vec2 texCoords;
uniform float time;
uniform sampler2D tex;
uniform sampler2D tex2;

const float PI = 3.14159265359;

void main() {
	FragColor = mix(texture(tex, texCoords), texture(tex2, texCoords), 0.8f);
	FragColor = FragColor * vec4(1f, sin(mod(time, 1) * PI), .2f, 1f);
}
