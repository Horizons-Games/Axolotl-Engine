#ifndef _METALLIC_MATERIALS_INCLUDE_
#define _METALLIC_MATERIALS_INCLUDE_

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

#endif