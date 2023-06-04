#version 460 

in vec3 fragPos;

out vec4 outColor;

void main()
{
	vec3 color = vec3(0.5, 0.0, 0.5);
	vec4 gammaCorrection = vec4(2.2);

	color.rgb = pow(color, vec3(1.0/gammaCorrection));
	outColor = vec4(color, 1.0);
}