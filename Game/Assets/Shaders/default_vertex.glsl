#version 440

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;

layout(location = 0) uniform mat4 proj;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 model;

out vec2 uv0;
out vec3 fragPos;
out vec3 fragNormal;
out vec3 fragTangent;

void main()
{
    //normals
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    fragNormal = normalize(normalMatrix * aNormal);
    fragTangent = normalize(normalMatrix * aTangent);


    fragPos = vec3(model * vec4(vertexPosition, 1.0));

    gl_Position = proj*view*model*vec4(vertexPosition, 1.0); 
	uv0 = vertex_uv0;
}