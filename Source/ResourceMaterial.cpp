#include "ResourceMaterial.h"
#include "EngineLog.h"
#include <GL/glew.h>

ResourceMaterial::ResourceMaterial()
	: shininess(32.f)
{
	this->diffuse = diffuse;
	this->specular = specular;
}

ResourceMaterial::~ResourceMaterial()
{
}

void ResourceMaterial::bind(unsigned int program)
{
	
	//glUniformMatrix4fv(glGetUniformLocation(program, "material.ambient"), 1, GL_TRUE, (const float*)&ambient);
	glUniform1i(glGetUniformLocation(program, "material.diffuse"), diffuse.id);
	glUniform1i(glGetUniformLocation(program, "material.specular"), specular.id);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);

}
