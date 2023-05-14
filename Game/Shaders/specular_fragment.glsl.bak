#version 440

#define PI 3.1415926535897932384626433832795
#define EPSILON 1e-5

struct Material {
    vec4 diffuse_color;         //location 3
    int has_diffuse_map;        //location 4
    float smoothness;           //location 5
    float normal_strength;      //location 6
    int has_normal_map;         //location 7
    vec3 specular_color;        //location 8
    int has_specular_map;       //location 9
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
	float outerAngle;	//4  //48   --> 52 
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
layout(binding = 7) uniform sampler2D specular_map;

uniform Light light;

in vec3 fragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;

in vec2 TexCoord;

out vec4 outColor;

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
    
    vec3 orthoTangent = normalize(tangent - dot(tangent, normal) * normal);
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

    return squareRoughness/max((PI*pow(squareNH*(squareRoughness-1.0)+1.0,2)),EPSILON);
}

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
        
        vec3 L = normalize(-directionalDir);
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

        vec3 L = normalize(FragPos - pos);
        vec3 H = (-L+V)/length(-L+V);
        float dotNL = max(dot(N, -L), EPSILON);

        vec3 FS = fresnelSchlick(f0, max(dot(L,H), EPSILON));
        float SV = smithVisibility(dotNL, max(dot(N,V), EPSILON), roughness);
        float GGXND = GGXNormalDistribution(max(dot(N,H), EPSILON), roughness);

        // Attenuation
        float distance = dot(FragPos - pos, aim);
        float maxValue = pow(max(1 - pow(distance/radius,4), 0),2);
        float attenuation = maxValue/(pow(distance,2) + 1);

        float C = dot(L, aim);
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
  
void main()
{
	vec3 norm = Normal;
    vec3 tangent = fragTangent;
    vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);
    vec4 gammaCorrection = vec4(2.2);

    // Diffuse
	vec4 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(diffuse_map, TexCoord); 
        //textureMat = pow(textureMat, vec3(2.2));
    }
    textureMat = pow(textureMat, gammaCorrection);
    textureMat.a = material.diffuse_color.a; //Transparency
    
    // Normals
	if (material.has_normal_map == 1)
	{
        mat3 space = CreateTangentSpace(norm, tangent);
        norm = texture(normal_map, TexCoord).rgb;
        norm = norm * 2.0 - 1.0;
        norm.xy *= material.normal_strength;
        norm = normalize(norm);
        norm = normalize(space * norm);
	}

    // Specular
    vec4 specularMat = vec4(material.specular_color, 1.0);
    if (material.has_specular_map == 1) {
        specularMat = vec4(texture(specular_map, TexCoord));
    }

    vec3 f0 = specularMat.rgb;

    // smoothness and roughness
    float smoothness = material.smoothness;
    float roughness = pow(1-material.smoothness,2) + EPSILON;
    
    // Lights
    vec3 ambient = ambientValue * textureMat.rgb;

    vec3 Lo = calculateDirectionalLight(norm, viewDir, textureMat.rgb, f0, roughness);

    if (num_point > 0)
    {
        Lo += calculatePointLights(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    if (num_spot > 0)
    {
        Lo += calculateSpotLights(norm, viewDir, textureMat.rgb, f0, roughness);
    }

    vec3 color = ambient + Lo;
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
   
    outColor = vec4(color, material.diffuse_color.a);
}