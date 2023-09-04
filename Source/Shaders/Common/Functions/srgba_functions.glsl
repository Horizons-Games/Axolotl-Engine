#ifndef _SRGBA_FUNCTIONS_INCLUDE_
#define _SRGBA_FUNCTIONS_INCLUDE_

#define GAMMA 2.2

vec4 SRGBA(vec4 color) {
	return vec4(pow(color.rgb, vec3(GAMMA)), color.a);
}

vec3 SRGB(vec3 color) {
	return pow(color.rgb, vec3(GAMMA));
}

#endif