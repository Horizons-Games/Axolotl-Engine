#version 440

out vec4 outColor;


layout(std140) uniform Ambient
{
	vec3 ambientValue;		//12	//0
};

layout(std140) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

uniform vec3 viewPos;
uniform vec3 lightPos;

in vec2 uv0;
in vec3 fragPos;
in vec3 fragNormal;
in vec3 fragTangent;

layout(binding = 0) uniform sampler2D diffuse;
layout(binding = 1) uniform sampler2D normalMap;

mat3 CreateTangentSpace(const vec3 normal, const vec3 tangent)
{
    vec3 orthoTangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(orthoTangent, normal);
    return transpose(mat3(orthoTangent, bitangent, normal)); //TBN
}

void main()
{
	vec3 normal = fragNormal;
    vec3 tangent = fragTangent;
	
	bool hasNormals = false;
	if (hasNormals)
	{
        mat3 space = CreateTangentSpace(normalize(normal), normalize(tangent));
        float nStrength = 1.0;
        normal = texture(normalMap, uv0).rgb;
        normal = normal * 2.0 - 1.0;
        normal.xy *= nStrength;
        normal = normalize(normal);
        normal = space * normal;
	}
    // get diffuse color
	vec3 color = texture2D(diffuse, uv0).rgb;
	// ambient
	vec3 ambient = 0.1 * color;

    // diffuse 
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = 0.8 * diff * color;
        
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;  
        
    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, 1.0);
}