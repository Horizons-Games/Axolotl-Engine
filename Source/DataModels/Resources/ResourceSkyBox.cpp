#include "ResourceSkyBox.h"

#include "GL/glew.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceTexture.h"

void ResourceSkyBox::Load()
{
	glGenTextures(1, &glTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture);

	for (int i = 0; i < texturesUIDs.size(); ++i)
	{
		std::shared_ptr<ResourceTexture> textI =
			std::dynamic_pointer_cast<ResourceTexture>(App->resources->RequestResource(texturesUIDs[i]));

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textI->GetInternalFormat(), textI->GetWidth(),
			textI->GetHeight(), 0, textI->GetFormat(), textI->GetImageType(), &(textI->GetPixels()));
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void ResourceSkyBox::Unload()
{
	for (UID uid : texturesUIDs)
	{
		App->resources->RequestResource(uid)->Unload();
	}
	//this will keep the capacity to 6
	texturesUIDs.clear();
	glDeleteTextures(1, &glTexture);
	glTexture = 0;
}
