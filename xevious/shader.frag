#version 430

// Global variables for lighting calculations
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform vec3 lightPos = vec3(3,3,3);
layout(location = 3) uniform vec3 color = vec3(1,0,1);
layout(location = 4) uniform sampler2D tex;
layout(location = 5) uniform bool useTexture = false;
// Per-vertex attributes


// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader
in vec3 fragPos;    // World-space position
in vec3 fragNormal; // World-space normal
in vec2 fragTexCoord;
in vec3 fragColor;

void main() {    
    vec3 l = normalize(lightPos - fragPos);
    float d = clamp(dot(l, fragNormal), 0, 1);
    
   // outColor = d * vec4(fragTexCoord[0] !=0 ? 0 : 1, 0,0.,1.);
    vec4 baseColor = vec4(fragColor * color, 1.0); //elements wise multiplication
    if (useTexture){
        baseColor = baseColor* texture(tex, fragTexCoord);
    }
    vec3 diffuse = d * vec3(baseColor);
    
    vec3 v = normalize(viewPos - fragPos);
    vec3 h = normalize(l + v);
    
    float ps = clamp(dot(fragNormal, h), 0, 1);
    vec3 specular =  vec3(1.0, 1.0, 1.0) * pow(ps , 16);
    vec3 result = diffuse + specular;

    result = clamp(result, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
    outColor = vec4(result, 1.0);
   // outColor = texture(tex, fragTexCoord);
}