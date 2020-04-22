#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aWeight;

uniform mat4 MVP;
uniform mat4 bone;

out vec2 texCoords;

void main() {
	texCoords = aTexCoords;

	vec4 pos = vec4(aPos, 1.0f);
	// if aWeight == 0, then pos = aPos
	pos = aWeight * bone * pos + (1.0f - abs(sign(aWeight))) * pos;
	gl_Position = MVP * pos;
}
