#version 460

#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

in vec2 TexCoord;

layout(binding = 0) uniform sampler2D scene;
layout(binding = 1) uniform sampler2D bloomBlur;
uniform int tonneMappingMode;
uniform int bloomActivation;

out vec4 outColor;

vec3 ACESFilm(in vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b))/(x * (c * x + d) + e), 0.0, 1.0);
}

vec3 Uncharted2Tonemap(in vec3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x * (A * x + C * B) + D * E)/(x * (A * x + B) + D * F)) - E/F;
}

vec3 uncharted2_tonemap(const vec3 hdr)
{
    const float W = 11.2;
    const vec3 whiteScale = 1.0/Uncharted2Tonemap(vec3(W));
    vec3 curr = Uncharted2Tonemap(2.0 * hdr);
    return curr * whiteScale;
}

void main()
{             
    vec3 hdrColor = texture(scene, TexCoord).rgb;      
    if (bloomActivation == 1)
    {
        vec3 bloomColor = texture(bloomBlur, TexCoord).rgb;
        hdrColor += bloomColor; // additive blending
    }
    
    // tone mapping
    vec3 result;
    if (tonneMappingMode == 0)
    {
        result = hdrColor.rgb / (hdrColor.rgb + vec3(1.0)); // Reinhard tone mapping
    }
    else if (tonneMappingMode == 1)
    {
        result = uncharted2_tonemap(hdrColor);
    }
    else if (tonneMappingMode == 2)
    {
        result = ACESFilm(hdrColor);
    }

    // gamma correct
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / GAMMA));

    outColor = vec4(result, 1.0);
}