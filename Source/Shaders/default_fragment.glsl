#version 440

#define M_PI 3.1415926535897932384626433832795
#define EPSILON 1e-5


struct Material {
    vec4 diffuse_color;         //location 3         
    float normal_strength;      //location 4
    float smoothness;           //location 5
    float metalness;            //location 6
    
    int has_diffuse_map;        //location 7
    int has_metallic_map;       //location 8
    int has_normal_map;        //location 9
    int has_smoothness_map;     //location 10
};

struct PointLight
{
	vec4 position;  	//16 //16	// xyz position+w radius
	vec4 color; 		//16 //16   // rgb colour+alpha intensity
};

struct SpotLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16	// rgb colour+alpha intensity
	vec3 aim;			//12 //32
	float innerAngle;	//4  //44
	float outerAngle;	//4  //48 
    float padding1;     //4  //52
    vec2  padding2;     //8  //56 --> 64
};

layout(std140, binding=1) uniform Ambient
{
	vec3 ambientValue;		//12	//0
};

layout(std140, binding=2) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

readonly layout(std430, binding=3) buffer PointLights
{
	uint num_point;			//4		//0
	PointLight points[]; 	//32	//16
};

readonly layout(std430, binding=4) buffer SpotLights
{
	uint num_spot;
	SpotLight spots[];
};

struct Light {
    vec3 position;
    vec3 color;
};

layout(location = 3) uniform Material material; // 0-9
layout(binding = 5) uniform sampler2D diffuse_map;
layout(binding = 6) uniform sampler2D normal_map;
layout(binding = 7) uniform sampler2D metallic_map;

uniform Light light;

in vec3 fragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;
in vec2 TexCoord;

out vec4 outColor;

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
    vec3 orthoTangent = normalize(tangent - max(dot(tangent, normal),EPSILON) * normal);
    vec3 bitangent = cross(orthoTangent, normal);
    return mat3(tangent, bitangent, normal); //TBN
}

vec3 fresnelSchlick(vec3 F0, float dotLH)
{
    return F0 + (1.0 - F0) * pow(1.0 - dotLH, 5.0);
}

float smithVisibility(float dotNL, float dotNV, float roughness)
{
    return 0.5/(dotNL*(dotNV*(1-roughness)+roughness)+dotNV*(dotNL*(1-roughness)+roughness));
}

float GGXNormalDistribution(float dotNH, float roughness)
{
    float squareRoughness = roughness*roughness;
    float squareNH = dotNH*dotNH;

    return squareRoughness/(PI*pow(squareNH*(squareRoughness-1.0)+1.0,2));
}

vec3 calculateDirectionalLight(vec3 N, vec3 V, vec3 Cd, vec3 f0, float roughness)
{
    vec3 L = normalize(-directionalDir);
    vec3 H = (L+V)/length(L+V);
    float dotNL = max(dot(N,L), 0.0);
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
        float dotNL = max(dot(N,-L), 0.0);

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
  
void main()
{
	vec3 norm = Normal;
    vec3 tangent = fragTangent;
    vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);
    vec4 gammaCorrection = vec4(2.2);

	vec4 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(diffuse_map, TexCoord);
    }
    textureMat = pow(textureMat, gammaCorrection);
    
    textureMat.a = material.diffuse_color.a; //Transparency
    
	if (material.has_normal_map)
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

    vec3 color = ambientValue*textureMat.rgb + Lo;      // ambient: ambientValue*textureMat
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
   
    outColor = vec4(color, textureMat.a);
}