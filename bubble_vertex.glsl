#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

out vec3 normal;

uniform mat4 projection;
uniform mat4 modelView;

void main() {
	mat3 normalMat = mat3(transpose(inverse(modelView)));
	normal = normalize(normalMat * aNormal);
	gl_Position = projection * modelView * vec4(aPos, 1.0);
}