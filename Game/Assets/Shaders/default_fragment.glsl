#version 440

struct Material {
    vec3 diffuse_color;
    sampler2D diffuse_map;
    vec3 specular_color;
    sampler2D specular_map;
    float shininess;
    sampler2D normal_map;
    float normal_strength;
    vec3 ambient;
    
    int has_diffuse_map;
    int has_specular_map;
    int shininess_alpha;
    int has_shininess_map;
    bool has_normal_map;
};

struct PointLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16   // rgb colour+alpha intensity
};

struct SpotLight
{
	vec4 position;  	//16 //0	// xyz position+w radius
	vec4 color; 		//16 //16	// rgb colour+alpha intensity
	vec3 aim;			//16 //32
	float innerAngle;	//4  //48
	float outerAngle;	//4  //52
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

//out vec4 color;

uniform Material material;
uniform Light light;

in vec3 fragTangent;
in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;

//layout(binding = 1) uniform sampler2D normalMap;

in vec2 TexCoord;

out vec4 outColor;

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
    vec3 orthoTangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(orthoTangent, normal);
    return transpose(mat3(orthoTangent, bitangent, normal)); //TBN
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
  
void main()
{
	//vec3 ambient = ambientValue * vec3(texture(texDiffuse, uv0));

	//vec3 result = ambient;
	
	//color = vec4(result, 1.0);

	// -------------------------------------
	
	vec3 norm = normalize(Normal);
    vec3 tangent = fragTangent;
    vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);

	vec3 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(material.diffuse_map, TexCoord).rgb; 
        textureMat = pow(textureMat, vec3(2.2));
    }
    

	if (material.has_normal_map)
	{
        mat3 space = CreateTangentSpace(normalize(norm), normalize(tangent));
        norm = texture(material.normal_map, TexCoord).rgb;
        norm = norm * 2.0 - 1.0;
        norm.xy *= material.normal_strength;
        norm = normalize(norm);
        norm = space * norm;
	}

    //fresnel
    vec4 specularMat =  vec4(material.specular_color, 0.0);
    if (material.has_specular_map == 1) {
        specularMat = vec4(texture(material.specular_map, TexCoord));
        specularMat = pow(specularMat, vec4(2.2));
    }

    vec3 f0 =  specularMat.rgb;

    // shininess
    float shininess = material.shininess;
    if (material.shininess_alpha == 1) {
	    shininess = exp2(specularMat.a * 7 + 1);
    }

    float NdotL = max(dot(norm, lightDir), 0.0);
	vec3 fresnel  = fresnelSchlick(NdotL, f0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0001), shininess);
    vec3 numerator = (shininess + 2) * fresnel * spec;
	vec3 specular = numerator / 2;


  	vec3 kS = f0;
	vec3 kD = vec3(1.0) - kS;
  	
    // diffuse 
    vec3 diffuse = kD * textureMat;
    
    //Lo
    vec3 Lo = (diffuse + specular) * light.color * NdotL;
    
    // ambient
    vec3 ambient = vec3(0.01) * textureMat;
    vec3 color = ambient + Lo;
    
	//hdr rendering
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
   
    outColor = vec4(color, 1.0);
}