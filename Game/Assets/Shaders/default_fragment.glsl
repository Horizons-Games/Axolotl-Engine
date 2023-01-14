#version 440

struct Material {
    vec3 diffuse_color;
    sampler2D diffuse_map;
    vec3 specular_color;
    sampler2D specular_map;
    float shininess;
    vec3 ambient;
    
    int has_diffuse_map;
    int has_specular_map;
    int shininess_alpha;
};

layout(std140) uniform Ambient
{
	vec3 ambientValue;		//12	//0
};

layout(std140) uniform Directional
{
	vec3 directionalDir;  	//12	//0
	vec4 directionalColor;	//16	//16     // note: alpha parameter of colour is the intensity 
};

struct Light {
    vec3 position;
    vec3 color;
};

uniform Material material;
uniform Light light;

in vec3 Normal;
in vec3 FragPos;
in vec3 ViewPos;

in vec2 TexCoord;

out vec4 outColor;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
  
void main()
{  
	vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);

	vec3 textureMat = material.diffuse_color;
    if (material.has_diffuse_map == 1) {
        textureMat = texture(material.diffuse_map, TexCoord).rgb; 
        textureMat = pow(textureMat, vec3(2.2));
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