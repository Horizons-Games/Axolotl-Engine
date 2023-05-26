#version 460
#extension GL_ARB_shading_language_include : require

#include "/Common/Functions/ibl_functions.glsl"

uniform sampler2D hdr;

in vec3 texcoords;

out vec4 fragColor;

void main() {
	vec3 dir = normalize(texcoords);
	vec2 uv = CartesianToEquirectangular(dir);
	fragColor = texture(hdr, uv);
}