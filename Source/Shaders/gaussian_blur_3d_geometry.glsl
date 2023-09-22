#version 460 core
   
#define CASCADES 3

layout(triangles, invocations = CASCADES) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoord;
} gs_in[]; 

out vec2 TexCoord;
    
void main()
{          
    for (int i = 0; i < 3; ++i)
    {
        TexCoord = gs_in[i].texCoord;
        gl_Position = gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
} 