#version 430

// Global variables for lighting calculations
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform vec3 lightPos = vec3(3,3,3);
layout(location = 3) uniform vec3 color;
layout(location = 4) uniform sampler2D tex;
layout(location = 5) uniform bool useTexture = false;
layout(location = 7) uniform sampler2D texShadow;
layout(location = 8) uniform mat4 lightMVP;
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

    vec4 fragLightCoord = lightMVP * vec4(fragPos, 1.0);

        // Divide by w because fragLightCoord are homogenous coordinates
        fragLightCoord.xyz /= fragLightCoord.w;

        // The resulting value is in NDC space (-1 to 1).
        //  we transfrom them to texture space (0 to 1)
        fragLightCoord.xyz = fragLightCoord.xyz * 0.5 + 0.5;

        // Depth of the fragment with respect to the light
        float fragLightDepth = fragLightCoord.z;

        // Shadow map coordinate corresponding to this fragment
        vec2 shadowMapCoord = fragLightCoord.xy;

        float shadowScalar = 1.0;
        if (!(shadowMapCoord.x < 0. || shadowMapCoord.x > 1. || shadowMapCoord.y < 0. || shadowMapCoord.y > 1.))
        {
             float shadowMapDepth = texture(texShadow, shadowMapCoord).x;
            if (shadowMapDepth + 0.001 < fragLightCoord.z)
               shadowScalar = 0.1;
     //TODO soft shadows? average?
        }

    float d = clamp(dot(l, fragNormal), 0, 1);
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
    result = result;

    result = clamp(result, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
    outColor = vec4(result * shadowScalar, 1.0);

}