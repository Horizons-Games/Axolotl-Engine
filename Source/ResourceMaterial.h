#pragma once
#include <string>
#include "Math/float3.h"

struct Texture
{
	unsigned id = 0;
	std::string path;
	unsigned width = 0;
	unsigned height = 0;
};


class ResourceMaterial
{

public:

	ResourceMaterial();
	~ResourceMaterial();
	unsigned GetDiffuseId();
	unsigned GetSpecularId();

	void bind(unsigned int program);

private:

	Texture diffuse;
	Texture specular;
	float3 diffuse_color = float3(1.0);
	float3 specular_color = float3(0.5, 0.3, 0.5);
	float shininess;

};
inline unsigned ResourceMaterial::GetDiffuseId()
{
	return diffuse.id;
}
inline unsigned ResourceMaterial::GetSpecularId()
{
	return specular.id;
}
