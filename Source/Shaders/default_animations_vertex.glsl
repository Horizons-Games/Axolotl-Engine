#version 440

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;

uniform vec3 viewPos;
uniform int hasBones;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in uvec4 bone_indices;
layout(location = 5) in vec4 bone_weights;

layout(std430, row_major, binding = 10) buffer Skinning
{
    mat4 palette[];
};

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 ViewPos;
out vec3 FragTangent;

void main()
{
    vec4 position = vec4(vertexPosition, 1.0);
    vec4 norm = vec4(normal, 0.0);

    // Bones
    if (hasBones > 0)
    {
        mat4 skinT = palette[bone_indices[0]]*bone_weights[0]+palette[bone_indices[1]]*bone_weights[1]+             
              palette[bone_indices[2]]*bone_weights[2]+palette[bone_indices[3]]*bone_weights[3];
        
        position = (skinT*vec4(vertexPosition, 1.0));
        norm  = (skinT*vec4(normal, 0.0));
    }

    gl_Position = proj*view*model*position; 

    FragPos = vec3(model * position);

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * norm.xyz);

    FragTangent = normalize(normalMatrix * aTangent);

    TexCoord = texCoord;
    ViewPos = viewPos;
}