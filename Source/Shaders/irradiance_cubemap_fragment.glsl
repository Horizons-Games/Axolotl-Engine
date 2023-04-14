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
	vec3 irradiance = texture(environment, normal).rgb;
	irradiance = irradiance / (irradiance + vec3(1.0));
    irradiance = pow(irradiance, vec3(1.0/gammaCorrection));
	//END TODO
	fragColor = vec4(irradiance, 1.0);
}