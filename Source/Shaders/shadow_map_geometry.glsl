#version 460 core
   
#define CASCADES 3

layout(triangles, invocations = CASCADES) in;
layout(triangle_strip, max_vertices = 3) out;
    
layout(std140, row_major, binding = 2) uniform LightSpaceMatrices 
{
    mat4 proj[CASCADES];    // 16 // 00 (column 0)
                            // 16 // 16 (column 1)
                            // 16 // 32 (column 2)
                            // 16 // 48 (column 3)
    mat4 view[CASCADES];    // 16 // 64 (column 0)
                            // 16 // 80 (column 1)
                            // 16 // 96 (column 2)
                            // 16 // 112 (column 3)
};
    
void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = 
            proj[gl_InvocationID] * view[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
} 