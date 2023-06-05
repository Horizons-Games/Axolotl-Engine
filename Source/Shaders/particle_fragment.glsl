#version 460 

in vec3 fragPos;
in vec4 fragColor;

out vec4 outColor;

void main()
{
	vec3 color = fragColor.rgb;
	vec4 gammaCorrection = vec4(2.2);

	color = pow(color, vec3(1.0/gammaCorrection));
	outColor = vec4(color, fragColor.a);
}