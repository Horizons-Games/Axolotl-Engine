#version 460

layout(binding = 0) uniform sampler2D scene;
layout(binding = 1) uniform sampler2D emissive;

uniform float threshold;

in vec2 TexCoord;

out vec4 bloom;

void main()
{
    vec4 color = texture(scene, TexCoord);
    
    float bright = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722) * threshold);
    vec4 emissiveColor = texture(emissive, TexCoord);

    if(bright > 1.0)
        bloom = vec4(color.rgb, 1.0);
    else
        bloom = vec4(0.0, 0.0, 0.0, 1.0);
    if (dot(emissiveColor.rgb, emissiveColor.rgb) > 0.0)
        bloom = vec4(emissiveColor.rgb, 1.0);
}