#ifndef _MATERIALS_INCLUDE_
#define _MATERIALS_INCLUDE_

struct MetallicMaterial {
    vec4 diffuse_color;         //0 //16
    int has_diffuse_map;        //16 //4
    int has_normal_map;         //20 //4
    int has_metallic_map;       //24 //4
    int has_emissive_map;       //28 //4
    int is_reflective;          //32 //4
    float smoothness;           //36 //4
    float metalness;            //40 //4
    float normal_strength;      //44 //4
    sampler2D diffuse_map;      //48 //8
    sampler2D normal_map;       //56 //8
    sampler2D metallic_map;     //64 //8
    sampler2D emissive_map;     //72 //8 --> 80
};

struct SpecularMaterial {
    vec4 diffuse_color;         //0  //16
    vec3 specular_color;        //16 //16
    int has_diffuse_map;        //32 //4
    int has_normal_map;         //36 //4
    int has_specular_map;       //40 //4
    int has_emissive_map;       //44 //4
    int is_reflective;          //48 //4
    float smoothness;           //52 //4
    float normal_strength;      //56 //4
    int padding;                //60 //4
    sampler2D diffuse_map;      //64 //8
    sampler2D normal_map;       //72 //8
    sampler2D specular_map;     //80 //8
    sampler2D emissive_map;     //88 //8 --> 96
};

#endif