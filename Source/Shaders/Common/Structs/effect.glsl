#ifndef _EFFECT_INCLUDE_
#define _EFFECT_INCLUDE_

struct Effect {
    vec4 color;         //0     //16
    int discardFrag;    //16    //4
    int padding1;       //20    //4
    vec2 padding2;      //28    //12 --> 32
};

#endif