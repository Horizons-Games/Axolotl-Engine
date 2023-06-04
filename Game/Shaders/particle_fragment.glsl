#version 460 

in vec3 fragPos;

out vec4 outColor;

void main()
{
	vec3 color = vec3(1.0, 0.0, 0.0);
	vec4 gammaCorrection = vec4(2.2);

	color = pow(color, vec3(1.0/gammaCorrection));
	
	outColor = vec4(color, 1.0);
}