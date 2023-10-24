#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Functions/srgba_functions.glsl"

#include "/Common/Structs/materials.glsl"

#include "/Common/Structs/lights.glsl"

#include "/Common/Structs/tiling.glsl"

#include "/Common/Structs/effect.glsl"

layout(std140, binding=1) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430, binding = 11) buffer Materials {
    SpecularMaterial materials[];
};

readonly layout(std430, binding = 12) buffer Tilings {
    Tiling tilings[];
};

readonly layout(std430, binding = 13) buffer Effects {
    Effect effects[];
};

// IBL
layout(binding = 8) uniform samplerCube diffuse_IBL;
layout(binding = 9) uniform samplerCube prefiltered_IBL;
layout(binding = 10) uniform sampler2D environmentBRDF;

uniform int numLevels_IBL;
uniform float cubemap_intensity;

in vec3 FragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;

in flat int InstanceIndex;

out vec4 outColor;

vec3 calculateDirectionalLight(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 L = normalize(-directionalDir);
    vec3 H = (L+V)/length(L+V);

    float dotNL = max(dot(N,L), EPSILON);

    vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
    float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
    float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

    return (Cd*f0+0.25*FS*SV*GGXND)*directionalColor.rgb*directionalColor.a*dotNL;
}
  
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

	vec3 norm = Normal;
    vec3 tangent = FragTangent;
    vec3 viewDir = normalize(ViewPos - FragPos);

    // Diffuse
	vec4 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(material.diffuse_map, newTexCoord); 
        //textureMat = pow(textureMat, vec3(2.2));
    }
    textureMat = SRGBA(textureMat);
    textureMat.rgb += effect.color;
    
    //Transparency
    textureMat.a = material.has_diffuse_map * textureMat.a + 
        (1.0f-material.has_diffuse_map) * material.diffuse_color.a;

    // Normals
	if (material.has_normal_map == 1)
	{
        mat3 space = CreateTangentSpace(norm, tangent);
        norm = texture(material.normal_map, newTexCoord).rgb;
        norm = norm * 2.0 - 1.0;
        norm.xy *= material.normal_strength;
        norm = normalize(norm);
        norm = space * norm;
	}
    norm = normalize(norm);

    // Specular
    vec4 specularMat = vec4(material.specular_color, 1.0);
    if (material.has_specular_map == 1) {
        specularMat = vec4(texture(material.specular_map, newTexCoord));
    }

    vec3 f0 = specularMat.rgb;

    // smoothness and roughness
    float smoothness = material.smoothness;
    float roughness = pow(1-material.smoothness,2) + EPSILON;
    
    // Lights
    vec3 R = reflect(-viewDir, norm);
    float NdotV = max(dot(norm, viewDir), EPSILON);
    vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, textureMat.rgb, f0, diffuse_IBL, prefiltered_IBL, 
        environmentBRDF, numLevels_IBL, vec4(0)) * cubemap_intensity;

    vec3 Lo = calculateDirectionalLight(norm, viewDir, textureMat.rgb, f0, roughness);

    vec3 color = ambient + Lo;

    //Emissive
    if (material.has_emissive_map == 1) 
    {
        color += vec3(texture(material.emissive_map, newTexCoord));
    }
    
    outColor = vec4(color, textureMat.a);
}