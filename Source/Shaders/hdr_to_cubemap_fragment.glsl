#version 440

#define PI 3.1415926535897932384626433832795

uniform sampler2D hdr;

in vec3 texCoords;

out vec4 fragColor;

vec2 CartesianToEquirectangular(in vec3 dir) {
	float phi = atan(dir.z, dir.x);
	phi = phi / (2.0 * PI) + 0.5;

	float theta = asin(dir.y);
	theta = theta / PI + 0.5;

	return vec2(phi, theta);
}

void main() {
	vec3 dir = normalize(texCoords);
	vec2 uv = CartesianToEquirectangular(dir);
	fragColor = texture(hdr, uv);
}