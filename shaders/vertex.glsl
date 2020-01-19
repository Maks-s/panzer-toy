#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 trans;

out vec2 texCoords;

void main() {
	texCoords = aTexCoords;
	gl_Position = trans * vec4(aPos, 1.f);
}
