#version 440

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

uniform sampler2D diffuse;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;  
uniform vec3 viewPos;
layout(binding=5) uniform sampler2D mytexture;

in vec2 uv0;

//out vec4 FragColor;
out vec4 color;
  
void main()
{
	//color = vec4(0.87, 0.98, 0.98, 1.0); 
	//FragColor = vec4(lightColor * objectColor, 1.0);
	//color = texture(diffuse, uv0);

    //vec4 textureColor = texture(diffuse, uv0);
    //vec4 objectColor = vec4(lightColor * objectColor, 1.0);
    //color = mix(textureColor, objectColor, 0.5);

    
    // ambient
    vec3 ambient = light.ambient * vec3(texture(mytexture, uv0));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position  - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(mytexture, uv0));  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * 
    vec3(texture(mytexture, uv0)); 
        
    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0);

}