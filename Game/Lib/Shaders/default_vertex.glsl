#version 460


layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;
//layout(location = 2) uniform buffer Transforms mat4 model[];
layout(location = 0) uniform mat4 proj;

//layout(std140, row_major, binding = 0) uniform Camera {
// mat4 proj;
// mat4 view;
//};
//readonly layout(std430, row_major, binding = 10) buffer Transforms {
// mat4 models[];
//};

//layout(location = 2) uniform mat4 models[];

uniform vec3 viewPos;

layout(location=0) in vec3 vertexPosition;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec3 ViewPos;
out vec3 fragTangent;

//out int flat instance_index;


void main()
{
    //instance_index = gl_BaseInstance;
    //mat4 model = models[gl_BaseInstance];

    FragPos = vec3(model * vec4(vertexPosition, 1.0));
    //normals
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * normal);
    fragTangent = normalize(normalMatrix * aTangent);

    gl_Position = proj*view*model*vec4(vertexPosition, 1.0); 

    TexCoord = texCoord;
    ViewPos = viewPos;
}