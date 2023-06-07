#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Structs/lights.glsl"

#include "/Common/Uniforms/lights_uniform.glsl"

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

layout(std140, binding=1) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430, binding=2) buffer PointLights
{
	uint num_point;			//4		//0
	PointLight points[]; 	//32	//16
};

readonly layout(std430, binding=3) buffer SpotLights
{
	uint num_spot;
	SpotLight spots[];
};

readonly layout(std430, binding=4) buffer AreaLightsSphere
{
	uint num_spheres;
	AreaLightSphere areaSphere[];
};

readonly layout(std430, binding=5) buffer AreaLightsTube
{
	uint num_tubes;
	AreaLightTube areaTube[];
};

readonly layout(std430, binding = 11) buffer Materials {
    Material materials[];
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

vec3 calculatePointLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_point; ++i)
    {
        vec3 pos = points[i].position.xyz;
        vec3 color = points[i].color.rgb;
        float radius = points[i].position.w;
        float intensity = points[i].color.a;
        
        vec3 L = normalize(pos-FragPos);
        vec3 H = (L+V)/length(L+V);

        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = length(FragPos - pos);
        float maxValue = pow(max(1 - pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        vec3 Li = color*intensity*attenuation;

        Lo += (Cd*f0+0.25*FS*SV*GGXND)*Li*dotNL;
    }

    return Lo;
}

vec3 calculateSpotLights(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_spot; ++i)
    {
        vec3 pos = spots[i].position.xyz;
        vec3 aim = spots[i].aim;
        vec3 color = spots[i].color.rgb;
        float radius = spots[i].position.w;
        float intensity = spots[i].color.a;
        float innerAngle = spots[i].innerAngle;
        float outerAngle = spots[i].outerAngle;

        float cosInner = cos(innerAngle);
        float cosOuter = cos(outerAngle);

        vec3 L = normalize(pos-FragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = dot(FragPos - pos, aim);
        float maxValue = pow(max(1 - pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        float C = dot(-L, aim);
        float Catt = 0;

        if (C > cosInner)
        {
            Catt = 1;
        }
        else if (cosInner > C && C > cosOuter)
        {
            Catt = (C - cosOuter)/(cosInner - cosOuter);
        }
    
        vec3 Li = color * intensity * attenuation * Catt;
            
        Lo += (Cd*f0+0.25*FS*SV*GGXND)*Li*dotNL;
    }

    return Lo;
}

vec3 calculateAreaLightSpheres(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_spheres; ++i)
    {
        vec3 sP = areaSphere[i].position.xyz;
        vec3 color = areaSphere[i].color.rgb;
        float sR = areaSphere[i].position.w;
        float intensity = areaSphere[i].color.a;
        float lightRadius = areaSphere[i].lightRadius;

        // calculate closest point light specular
        vec3 oldL = sP - FragPos;
        vec3 R = reflect(-V, N);
        vec3 centerToRay = FragPos + max(dot(oldL, R), EPSILON) * R - sP;
        vec3 closest = sP + centerToRay * min(sR/length(centerToRay),1.0);

        vec3 L = normalize(closest - FragPos);
        vec3 H = normalize(L + V);
        float specularDotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(specularDotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation from the closest point view
        float distance = length(FragPos-closest);
        float maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);
        
        // calculate closest point light diffuse
        closest = sP;
        
        L = normalize(closest-FragPos);
        float diffuseDotNL = max(dot(N,L), EPSILON);

        vec3 Li = color*intensity;
        vec3 LoSpecular = 0.25 * FS * SV * GGXND * Li * attenuation * specularDotNL;
        vec3 LoDiffuse = (Cd) * Li * attenuation * diffuseDotNL;
        Lo += LoDiffuse + LoSpecular;
    }

    return Lo;
}

vec3 calculateAreaLightTubes(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 Lo = vec3(0.0);

    for (int i = 0; i < num_tubes; ++i)
    {
        vec3 posA = areaTube[i].positionA.xyz;
        vec3 posB = areaTube[i].positionB.xyz;
        vec3 color = areaTube[i].color.rgb;
        float intensity = areaTube[i].color.a;
        float lightRadius = areaTube[i].lightRadius;
        float tubeRadius = areaTube[i].positionA.w;

        // calculate closest point light specular
        vec3 PA = posA - FragPos;
        vec3 AB = posB - posA;

        vec3 R = reflect(-V, N);

        float dotABRefle = dot(AB, R);
        float num = dot(R, PA) * dotABRefle - dot(AB, PA);
        float denom = dot(AB, AB) - dotABRefle * dotABRefle;
        float t = clamp(num/denom, 0.0f, 1.0f);
        
        vec3 closest = posA + AB * t;

        vec3 oldL = closest - FragPos;
        vec3 centerToRay = FragPos + max(dot(oldL, R), EPSILON) * R - closest;
        closest = closest + centerToRay * min(tubeRadius/length(centerToRay),1.0);

        vec3 L = normalize(closest-FragPos);
        vec3 H = normalize(L + V);

        float specularDotNL = max(dot(N,L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(specularDotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation from the closest point view
        float distance = length(closest-FragPos);
        float maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        // calculate closest point light diffuse
        float a = length(posA-FragPos);
        float b = length(posB-FragPos);
        float x = (a)/(b + a);
        closest = posA + AB * x;

        L = normalize(closest-FragPos);
        float diffuseDotNL = max(dot(N,L), EPSILON);

        vec3 Li = color * intensity;
        vec3 LoSpecular = (0.25 * FS * SV * GGXND) * Li * attenuation * specularDotNL;
        vec3 LoDiffuse = (Cd) * Li * attenuation * diffuseDotNL;
        Lo += LoDiffuse + LoSpecular;
    }

    return Lo;
}
  
void main()
{
    Material material = materials[InstanceIndex];

	vec3 norm = Normal;
    vec3 tangent = FragTangent;
    vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);
    vec4 gammaCorrection = vec4(2.2);

    // Diffuse
	vec4 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(material.diffuse_map, TexCoord); 
        //textureMat = pow(textureMat, vec3(2.2));
    }
    textureMat = pow(textureMat, gammaCorrection);
    textureMat.a = material.diffuse_color.a; //Transparency
    
    // Normals
	if (material.has_normal_map == 1)
	{
        mat3 space = CreateTangentSpace(norm, tangent);
        norm = texture(material.normal_map, TexCoord).rgb;
        norm = norm * 2.0 - 1.0;
        norm.xy *= material.normal_strength;
        norm = normalize(norm);
        norm = space * norm;
	}
    norm = normalize(norm);

    // Specular
    vec4 specularMat = vec4(material.specular_color, 1.0);
    if (material.has_specular_map == 1) {
        specularMat = vec4(texture(material.specular_map, TexCoord));
    }

    vec3 f0 = specularMat.rgb;

    // smoothness and roughness
    float smoothness = material.smoothness;
    float roughness = pow(1-material.smoothness,2) + EPSILON;
    
    // Lights
    vec3 R = reflect(-viewDir, norm);
    float NdotV = max(dot(norm, viewDir), EPSILON);
    vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, textureMat.rgb, f0, diffuse_IBL, prefiltered_IBL, environmentBRDF, numLevels_IBL) * cubemap_intensity;

    vec3 Lo = calculateDirectionalLight(norm, viewDir, textureMat.rgb, f0, roughness);

    if (num_point > 0)
    {
        Lo += calculatePointLights(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_spot > 0)
    {
        Lo += calculateSpotLights(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_spheres > 0)
    {
        Lo += calculateAreaLightSpheres(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_tubes > 0)
    {
        Lo += calculateAreaLightTubes(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    vec3 color = ambient + Lo;
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
   
    outColor = vec4(color, material.diffuse_color.a);
}