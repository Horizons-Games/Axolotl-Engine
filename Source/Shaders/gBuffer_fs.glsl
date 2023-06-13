#version 460

#extension GL_ARB_bindless_texture : require

struct Material {
    vec4 diffuse_color;         //0  //16
    vec3 specular_color;        //16 //16       
    int has_diffuse_map;        //32 //4
    int has_normal_map;         //36 //4
    int has_specular_map;       //40 //4
    float smoothness;           //44 //4
    float normal_strength;      //48 //4
    sampler2D diffuse_map;      //48 //8
    sampler2D normal_map;       //56 //8
    sampler2D specular_map;     //64 //8    
    vec2 padding;               //72 //8 --> 80
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

    //Diffuse
    gDiffuse = material.diffuse_color;
    if (material.has_diffuse_map == 1)
    {
        gDiffuse = texture(material.diffuse_map, TexCoord);
    }
    //Transparency
    gDiffuse.a = material.has_diffuse_map * gDiffuse.a + 
        (1.0f-material.has_diffuse_map) * material.diffuse_color.a;
    
    //Specular + smoothness
    gSpecular = vec4(material.specular_color, material.smoothness);
    if (material.has_specular_map == 1) {
        gSpecular = vec4(texture(material.specular_map, TexCoord));
    }
} 