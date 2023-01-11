#include "ResourceMaterial.h"
#include "ModuleTexture.h"
#include <GL/glew.h>

ResourceMaterial::ResourceMaterial()
	: shininess(32.f)
{

}

ResourceMaterial::~ResourceMaterial()
{
	//Need Unload method from Texture to unload diffuse and specular
}

void ResourceMaterial::bind(unsigned int program)
{
	glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), GetDiffuseId());
	glUniform1i(glGetUniformLocation(program, "material.specular_map"), GetSpecularId());
	glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);
}
