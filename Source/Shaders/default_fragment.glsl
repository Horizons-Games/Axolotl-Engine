#version 460

#extension GL_ARB_bindless_texture : require
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/pbr_functions.glsl"

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

struct PointLight
{
	vec4 position;  	//16 //16	// xyz position+w radius
	vec4 color; 		//16 //16   // rgb colour+alpha intensity
};

struct SpotLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16  	// rgb colour+alpha intensity
	vec3 aim;			//12 //32
	float innerAngle;	//4  //44
	float outerAngle;	//4  //48 
    float padding1;     //4  //52
    vec2  padding2;     //8  //56 --> 64
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

struct Light {
    vec3 position;
    vec3 color;
};

readonly layout(std430, binding = 11) buffer Materials {
    Material materials[];
};

// IBL
layout(binding = 8) uniform samplerCube diffuse_IBL;
layout(binding = 9) uniform samplerCube prefiltered_IBL;
layout(binding = 10) uniform sampler2D environmentBRDF;

uniform int numLevels_IBL;
uniform float cubeMap_intensity;
uniform Light light;

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

        vec3 L = normalize(pos-FragPos);
        vec3 H = (L+V)/length(L+V);
        float dotNL = max(dot(N,L), EPSILON);

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
    if (material.has_diffuse_map == 1)
    {
        textureMat = texture(material.diffuse_map, TexCoord);
    }
    textureMat = pow(textureMat, gammaCorrection); // sRGB textures to linear space
    
    if(textureMat.a < 0.01)
    {
        discard;
    }

    textureMat.a = material.diffuse_color.a; //Transparency
    
    // Normals
	if (material.has_normal_map == 1)
	{
        mat3 space = CreateTangentSpace(norm, tangent);
        norm = texture(material.normal_map, TexCoord).rgb;
        norm = norm * 2.0 - 1.0;
        norm.xy *= material.normal_strength;
        norm = normalize(norm);
        norm = normalize(space * norm);
	}
    
    // Metallic
    vec4 colorMetallic = texture(material.metallic_map, TexCoord);
    float metalnessMask = material.has_metallic_map * colorMetallic.r + (1 - material.has_metallic_map) * 
        material.metalness;

    vec3 Cd = textureMat.rgb*(1.0-metalnessMask);
    vec3 f0 = mix(vec3(0.04), textureMat.rgb, metalnessMask);

    // smoothness and roughness
    float roughness = pow(1-material.smoothness,2) + EPSILON;
    if (material.has_metallic_map == 1)
	{
        roughness = pow(1.0 * colorMetallic.a,2) + EPSILON;
    }

    // Lights
    vec3 Lo = calculateDirectionalLight(norm, viewDir, Cd, f0, roughness);

    if (num_point > 0)
    {
        Lo += calculatePointLights(norm, viewDir, Cd, f0, roughness);
    }

    if (num_spot > 0)
    {
        Lo += calculateSpotLights(norm, viewDir, Cd, f0, roughness);
    }

    vec3 R = reflect(-viewDir, norm);
    float NdotV = max(dot(norm, viewDir), EPSILON);
    vec3 ambient = GetAmbientLight(norm, R, NdotV, roughness, Cd, f0, diffuse_IBL, prefiltered_IBL, environmentBRDF, 
        numLevels_IBL) * cubeMap_intensity;

    vec3 color = ambient + Lo;
    //vec3 color = ambient;
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/gammaCorrection));
   
    outColor = vec4(color, textureMat.a);
}