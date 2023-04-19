#version 440

in vec3 texcoords;
uniform samplerCube environment;

out vec4 fragColor;

void main()
{
	vec4 gammaCorrection = vec4(2.2);
	vec3 normal = normalize(texcoords);
	// TODO: Approximate irradiance integral around hemisphere
	// defined by normal
	
	// Here needs to be the MonteCarlo integration
	vec3 irradiance = vec3(0.0);
	
	fragColor = vec4(irradiance, 1.0);
}