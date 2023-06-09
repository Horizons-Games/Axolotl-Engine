#version 460

#extension GL_ARB_bindless_texture : require

struct Material {
    vec4 diffuse_color;         //0 //16
    int has_diffuse_map;        //16 //4       
    int has_normal_map;         //20 //4
    int has_metallic_map;       //24 //4
    float smoothness;           //28 //4
    float metalness;            //32 //4
    float normal_strength;      //36 //4
    sampler2D diffuse_map;      //40 //8
    sampler2D normal_map;       //48 //8
    sampler2D metallic_map;     //56 //8 --> 64
};

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gSpecular;

readonly layout(std430, binding = 11) buffer Materials {
    Material materials[];
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

in flat int InstanceIndex;

void main()
{    

    Material material = materials[InstanceIndex];

    gPosition = FragPos;
    gNormal = normalize(Normal);
    gDiffuse = material.diffuse_color;
    if (material.has_diffuse_map == 1)
    {
        gDiffuse = texture(material.diffuse_map, TexCoord);
    }
    gSpecular = texture(material.metallic_map, TexCoord);
} 