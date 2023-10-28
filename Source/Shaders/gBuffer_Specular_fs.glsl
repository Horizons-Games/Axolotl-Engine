#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Functions/srgba_functions.glsl"

#include "/Common/Structs/materials.glsl"

#include "/Common/Structs/effect.glsl"

#include "/Common/Structs/tiling.glsl"

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gSpecular;
layout (location = 4) out vec4 gEmissive;

readonly layout(std430, binding = 11) buffer Materials {
    SpecularMaterial materials[];
};

readonly layout(std430, binding = 12) buffer Tilings {
    Tiling tilings[];
};

readonly layout(std430, binding = 13) buffer Effects {
    Effect effects[];
};

in vec3 FragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;

in flat int InstanceIndex;

void main()
{    
    SpecularMaterial material = materials[InstanceIndex];
    Effect effect = effects[InstanceIndex];

    if (effect.discardFrag == 1)
    {
        discard;
        return;
    }

    Tiling tiling = tilings[InstanceIndex];

    vec2 newTexCoord = TexCoord*tiling.percentage*tiling.tiling+tiling.offset;

    gPosition = FragPos;
    gNormal = Normal;

    //Diffuse
    gDiffuse = material.diffuse_color;
    if (material.has_diffuse_map == 1)
    {
        gDiffuse = texture(material.diffuse_map, newTexCoord);
    }
    gDiffuse = SRGBA(gDiffuse);
    gDiffuse += effect.color;

    //Normals
    if (material.has_normal_map == 1)
	{
        mat3 space = CreateTangentSpace(gNormal, FragTangent);
        gNormal = texture(material.normal_map, newTexCoord).rgb;
        gNormal = gNormal * 2.0 - 1.0;
        gNormal.xy *= material.normal_strength;
        gNormal = normalize(gNormal);
        gNormal = normalize(space * gNormal);
	}
    
    //Specular + smoothness
    gSpecular = vec4(material.specular_color, material.smoothness);
    if (material.has_specular_map == 1) {
        gSpecular = vec4(texture(material.specular_map, newTexCoord));
    }

    //Emissive
    gEmissive= vec4(0.0);
    if (material.has_emissive_map == 1) 
    {
        gEmissive.rgb = vec3(texture(material.emissive_map, newTexCoord).rgb);
    }
} 