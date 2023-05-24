#version 460

struct PerInstance {
    uint  numBones;
    uint  paletteOffset;
    uint  padding0, padding1;
};

layout(std140, row_major, binding = 0) uniform Camera 
{
    mat4 proj;      // 16 // 00 (column 0)
                    // 16 // 16 (column 1)
                    // 16 // 32 (column 2)
                    // 16 // 48 (column 3)
    mat4 view;      // 16 // 64 (column 0)
                    // 16 // 80 (column 1)
                    // 16 // 96 (column 2)
                    // 16 // 112 (column 3)
};

layout(std430, row_major, binding = 7) buffer Skinning
{
    mat4 palette[];
};

readonly layout(std430, row_major, binding = 10) buffer Transforms 
{
    mat4 models[];
};

readonly layout(std430, binding = 6) buffer PerInstances {
    PerInstance instanceInfo[];
};

uniform vec3 viewPos;

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in uvec4 bone_indices;
layout (location = 5) in vec4 bone_weights;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 ViewPos;
out vec3 FragTangent;

out flat int InstanceIndex;

void main()
{
    mat4 model = models[gl_BaseInstance];
    PerInstance perInstance = instanceInfo[gl_BaseInstance];

    vec4 position = vec4(vertexPosition, 1.0);
    vec4 norm = vec4(normal, 0.0);

    // Skinning
    if (perInstance.numBones > 0)
    {
        mat4 skinT = palette[perInstance.paletteOffset + bone_indices[0]]*bone_weights[0]+
                     palette[perInstance.paletteOffset + bone_indices[1]]*bone_weights[1]+             
                     palette[perInstance.paletteOffset + bone_indices[2]]*bone_weights[2]+
                     palette[perInstance.paletteOffset + bone_indices[3]]*bone_weights[3];
        
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

    InstanceIndex = gl_BaseInstance;
}