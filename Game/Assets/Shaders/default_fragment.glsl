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
    int has_shininess_map;
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
	vec3 textureMat = texture(material.diffuse_map, TexCoord).rgb;
    textureMat = pow(textureMat, vec3(2.2));
    
    //fresnel
    //vec3 f0 =  vec3(texture(material.specular_map, TexCoord));
    vec3 f0 =  vec3(0.5, 0.3, 0.5);
    float NdotL = max(dot(norm, lightDir), 0.0);
	vec3 fresnel  = fresnelSchlick(NdotL, f0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0001), material.shininess);
    vec3 numerator = (material.shininess + 2) * fresnel * spec;
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