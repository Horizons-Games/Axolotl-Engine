#include "ResourceCubemap.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "GL/glew.h"
#include "DataModels/Resources/ResourceTexture.h"

ResourceCubemap::ResourceCubemap(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
    const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceCubemap::~ResourceCubemap()
{
    Unload();
}

void ResourceCubemap::InternalLoad()
{
    glGenTextures(1, &enviromentTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, enviromentTexture);

   /* for (int i = 0; i < textures.size(); ++i)
    {
        std::shared_ptr<ResourceTexture> textI = textures[i];

        if (textI)
        {
            textI->Load();
            std::vector<uint8_t> aux = textI->GetPixels();
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textI->GetInternalFormat(), textI->GetWidth(),
                textI->GetHeight(), 0, textI->GetFormat(), textI->GetImageType(), &(aux[0]));
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        LoadVBO();
        CreateVAO();
    }*/
}

void ResourceCubemap::InternalUnload()
{
    glDeleteTextures(1, &enviromentTexture);
    enviromentTexture = 0;
}

bool ResourceCubemap::ChildChanged() const
{
    bool result = false;
    if (hdrTexture && hdrTexture->IsChanged())
    {
        result = true;
        hdrTexture->SetChanged(false);
    }
    return result;
}