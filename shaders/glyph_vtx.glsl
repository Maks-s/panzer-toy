#version 440 core
// aInfos = {pos x, pos y, tex x, tex y}
layout (location = 0) in vec4 aInfos;

uniform mat4 MVP;

out vec2 texCoords;

void main() {
	texCoords = aInfos.zw;
	gl_Position = MVP * vec4(aInfos.xy, 1.0f, 1.0f);
}
