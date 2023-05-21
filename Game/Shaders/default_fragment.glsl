#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

#include "/Common/Structs/lights.glsl"

#include "/Common/Uniforms/lights_uniform.glsl"

struct Material {
    vec4 diffuse_color;         //location 3
    int has_diffuse_map;        //location 4
    float smoothness;           //location 5
    float normal_strength;      //location 6
    int has_normal_map;         //location 7
    float metalness;            //location 8
    int has_metallic_map;       //location 9
};

layout(std140, binding=0) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430, binding=1) buffer PointLights
{
	uint num_point;			//4		//0
	PointLight points[]; 	//32	//16
};

readonly layout(std430, binding=2) buffer SpotLights
{
	uint num_spot;
	SpotLight spots[];
};

readonly layout(std430, binding=3) buffer AreaLightsSphere
{
	uint num_spheres;
	AreaLightSphere areaSphere[];
};

readonly layout(std430, binding=4) buffer AreaLightsTube
{
	uint num_tubes;
	AreaLightTube areaTube[];
};

layout(location = 3) uniform Material material; // 0-9
layout(binding = 5) uniform sampler2D diffuse_map;
layout(binding = 6) uniform sampler2D normal_map;
layout(binding = 7) uniform sampler2D metallic_map;

// IBL
layout(binding = 8) uniform samplerCube diffuse_IBL;
layout(binding = 9) uniform samplerCube prefiltered_IBL;
layout(binding = 10) uniform sampler2D environmentBRDF;
uniform int numLevels_IBL;

in vec3 FragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;

out vec4 outColor;

vec3 calculateDirectionalLight(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 L = normalize(-directionalDir);
    vec3 H = (L+V)/length(L+V);

    float dotNL = max(dot(N,L), EPSILON);

    vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
    float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
    float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

    return (Cd*(1-f0)+0.25*FS*SV*GGXND)*directionalColor.rgb*directionalColor.a*dotNL;
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

        vec3 L = normalize(FragPos-pos);
        vec3 H = (-L+V)/length(-L+V);

        float dotNL = max(dot(N,-L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = length(FragPos-pos);
        float maxValue = pow(max(1-pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        vec3 Li = color*intensity*attenuation;

        Lo += (Cd*(1-f0)+0.25*FS*SV*GGXND)*Li*dotNL;
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

        vec3 L = normalize(FragPos-pos);
        vec3 H = (-L+V)/length(-L+V);
        float dotNL = max(dot(N,-L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = dot(FragPos - pos, aim);
        float maxValue = pow(max(1 - pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        float C = dot(L, aim);
        float Catt = 0.0;

        if (C > cosInner)
        {
            Catt = 1.0;
        }
        else if (cosInner > C && C > cosOuter)
        {
            Catt = (C-cosOuter)/(cosInner-cosOuter);
        }
    
        vec3 Li = color*intensity*attenuation*Catt;
            
        Lo += (Cd*(1-f0)+0.25*FS*SV*GGXND)*Li*dotNL;
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

        // Attenuation Specular
        float distance = length(FragPos-closest);
        float maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuationSpecular = maxValue/(pow(distance,2) + 1);
        
        // calculate closest point light diffuse
        closest = sP;
        
        // Attenuation Diffuse
        distance = length(closest-FragPos);
        maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuationDiffuse = maxValue/(pow(distance,2) + 1);

        L = normalize(closest-FragPos);
        float diffuseDotNL = max(dot(N,L), EPSILON);

        vec3 Li = color*intensity;
        vec3 LoSpecular = 0.25 * FS * SV * GGXND * Li * attenuationSpecular * specularDotNL;
        vec3 LoDiffuse = (Cd * (1 - f0)) * Li * attenuationDiffuse * diffuseDotNL;
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
        float distAB = length(AB);
        float denom = distAB*distAB - dotABRefle * dotABRefle;
        //float denom = dot(AB, AB) - dotABRefle * dotABRefle;
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

        // Attenuation Specular
        float distance = length(closest-FragPos);
        float maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuationSpecular = maxValue/(pow(distance,2) + 1);

        // calculate closest point light diffuse
        float a = length(posA-FragPos);
        float b = length(posB-FragPos);
        float c = length(AB);
        float x = (c * a)/(b + a);
        closest = posA + AB * x;

        L = normalize(closest-FragPos);
        float diffuseDotNL = max(dot(N,L), EPSILON);

        // Attenuation Diffuse
        distance = length(closest-FragPos);
        maxValue = pow(max(1-pow(distance/lightRadius,4), 0),2);
        float attenuationDiffuse = maxValue/(pow(distance,2) + 1);

        vec3 Li = color * intensity;
        vec3 LoSpecular = (0.25 * FS * SV * GGXND) * Li * attenuationSpecular * specularDotNL;
        vec3 LoDiffuse = (Cd * (1 - f0)) * Li * attenuationDiffuse * diffuseDotNL;
        Lo += LoDiffuse + LoSpecular;
    }

    return Lo;
}
  
void main()
{
	vec3 norm = Normal;
    vec3 tangent = FragTangent;
    vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);
    vec4 gammaCorrection = vec4(2.2);

	vec4 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(diffuse_map, TexCoord);
    }
    textureMat = pow(textureMat, gammaCorrection); // sRGB textures to linear space
    
    textureMat.a = material.diffuse_color.a; //Transparency
    
	if (material.has_normal_map == 1)
	{
        mat3 space = CreateTangentSpace(norm, tangent);
        norm = texture(normal_map, TexCoord).rgb;
        norm = norm * 2.0 - 1.0;
        norm.xy *= material.normal_strength;
        norm = normalize(norm);
        norm = normalize(space * norm);
	}
    
    vec4 colorMetallic = texture(metallic_map, TexCoord);
    float metalnessMask = material.has_metallic_map * colorMetallic.r + (1 - material.has_metallic_map) * material.metalness;

    vec3 Cd = textureMat.rgb*(1.0-metalnessMask);
    vec3 f0 = mix(vec3(0.04), textureMat.rgb, metalnessMask);
    float roughness = pow(1-material.smoothness,2) + EPSILON;
    //float roughness = (1 - material.smoothness * (1.0 * colorMetallic.a)) * (1 - material.smoothness * (1.0 * colorMetallic.a)) + EPSILON;

    vec3 Lo = calculateDirectionalLight(norm, viewDir, Cd, f0, roughness);

    if (num_point > 0)
    {
        Lo += calculatePointLights(norm, viewDir, Cd, f0, roughness);
    }

    if (num_spot > 0)
    {
        Lo += calculateSpotLights(norm, viewDir, Cd, f0, roughness);
    }

    if (num_spheres > 0)
    {
        Lo += calculateAreaLightSpheres(norm, viewDir, Cd, f0, roughness);
    }

    if (num_tubes > 0)
    {
        Lo += calculateAreaLightTubes(norm, viewDir, Cd, f0, roughness);
    }

    vec3 R = reflect(-viewDir, norm);
    float NdotV = max(dot(norm, viewDir), EPSILON);
    vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, Cd, f0, diffuse_IBL, prefiltered_IBL, environmentBRDF, numLevels_IBL);
    vec3 color = ambient + Lo;
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gammaCorrection));
   
    outColor = vec4(color, textureMat.a);
}