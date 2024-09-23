#version 330 core

in vec3 Normal;  // Normal vector passed from vertex shader
in vec3 FragPos; // Fragment position passed from vertex shader

out vec4 color;  // Output color of the fragment

void main() {
    // Normalize the normal vector
    vec3 normalizedNormal = normalize(Normal);

    // Convert normal vector to a color. Scale to [0, 1] range by adding 0.5
    // Normalize the vector to get values between -1 and 1, and then offset
    // to [0, 1] range by adding 0.5 and scaling to [0, 1]
    color = vec4(normalizedNormal * 0.5 + 0.5, 1.0); // RGB and alpha = 1.0
}