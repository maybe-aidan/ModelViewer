#version 330 core

out vec4 FragCol;
in vec3 normal;

uniform vec3 color;
uniform float alpha;

void main() {
	FragCol = vec4(color, alpha);
}