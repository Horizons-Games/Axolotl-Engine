#ifndef _CASCADE_LIGHT_MATRICES_
#define _CASCADE_LIGHT_MATRICES_

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

#endif