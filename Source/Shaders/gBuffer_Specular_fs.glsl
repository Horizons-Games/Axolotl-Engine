#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Functions/srgba_functions.glsl"

struct Material {
    vec4 diffuse_color;         //0  //16
    vec3 specular_color;        //16 //16       
    int has_diffuse_map;        //32 //4
    int has_normal_map;         //36 //4
    int has_specular_map;       //40 //4
    int has_emissive_map;       //44 //4
    float smoothness;           //48 //4
    float normal_strength;      //52 //4
	float intensityBloom;		//56 //4
    sampler2D diffuse_map;      //64 //8
    sampler2D normal_map;       //72 //8
    sampler2D specular_map;     //80 //8    
    sampler2D emissive_map;     //88 //8 --> 96
};

struct Tiling {
    vec2 tiling;                //0  //8
    vec2 offset;                //8  //8 
    vec2 percentage;            //16 //8
    vec2 padding;               //24 //8 --> 32
};

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gSpecular;
layout (location = 4) out vec4 gEmissive;

readonly layout(std430, binding = 11) buffer Materials {
    Material materials[];
};

readonly layout(std430, binding = 12) buffer Tilings {
    Tiling tilings[];
};

in vec3 FragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;

in flat int InstanceIndex;

void main()
{    

    Material material = materials[InstanceIndex];
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
        gEmissive = vec4(texture(material.emissive_map, newTexCoord).rgb, material.intensityBloom / 5);
    }
} 