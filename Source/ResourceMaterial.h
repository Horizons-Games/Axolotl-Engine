#pragma once

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
	void setDiffuse(Texture diffuse);
	void setSpecular(Texture specular);
	void setShininess(float shininess);

	void bind(unsigned int program);

private:

	Texture diffuse;
	Texture specular;
	float shininess;

};

inline void ResourceMaterial::setDiffuse(Texture diffuse)
{
	this->diffuse = diffuse;
}

inline void ResourceMaterial::setSpecular(Texture specular)
{
	this->specular = specular;
}

inline void ResourceMaterial::setShininess(float shininess)
{
	this->shininess = shininess;
}

