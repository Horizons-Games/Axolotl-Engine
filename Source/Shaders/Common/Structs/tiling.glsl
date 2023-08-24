#ifndef _TILING_INCLUDE_
#define _TILING_INCLUDE_

struct Tiling {
    vec2 tiling;                //0  //8
    vec2 offset;                //8  //8 
    vec2 percentage;            //16 //8
    vec2 padding;               //24 //8 --> 32
};

#endif