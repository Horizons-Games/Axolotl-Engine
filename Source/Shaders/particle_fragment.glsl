#version 460 

in vec2 fragUv0;
in vec3 fragPos;
in vec4 fragColor;

layout(location = 3) uniform sampler2D diffuse;

out vec4 outColor;

void main()
{
	vec4 diffuse = texture(diffuse, fragUv0);

	vec3 color = fragColor.rgb;
	vec4 gammaCorrection = vec4(2.2);

	color = pow(color, vec3(1.0/gammaCorrection));
	outColor = vec4(color, fragColor.a);
}