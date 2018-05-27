#version 430

// Global variables for lighting calculations
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform vec3 lightPos = vec3(3,3,3);
layout(location = 3) uniform vec3 color = vec3(1,0,1);

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos;    // World-space position
in vec3 fragNormal; // World-space normal

void main() {
	outColor = vec4(color, 1.0);
}