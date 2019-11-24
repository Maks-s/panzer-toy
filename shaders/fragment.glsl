#version 440 core
out vec4 FragColor;

uniform float time;

const float PI = 3.14159265359;

void main() {
	FragColor = vec4(1f, sin(mod(time, 1) * PI), .2f, 1f);
}
