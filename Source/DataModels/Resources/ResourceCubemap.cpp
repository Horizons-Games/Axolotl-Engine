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
    //glGenTextures(1, &enviromentTexture);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, enviromentTexture);

    //for (unsigned int i = 0; i < 6; ++i)
    //{
    //    // note that we store each face with 16 bit floating point values
    //    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
    //        512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    //}
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //for (int i = 0; i < textures.size(); ++i)
    //{
    //    std::shared_ptr<ResourceTexture> textI = textures[i];

    //    if (textI)
    //    {
    //        textI->Load();
    //        std::vector<uint8_t> aux = textI->GetPixels();
    //        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textI->GetInternalFormat(), textI->GetWidth(),
    //            textI->GetHeight(), 0, textI->GetFormat(), textI->GetImageType(), &(aux[0]));
    //    }

    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //    LoadVBO();
    //    CreateVAO();
    //}
}

void ResourceCubemap::InternalUnload()
{
    //this will keep the capacity to 6
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


void ResourceCubemap::LoadVBO()
{
    //float skyboxVertices[] = {
    //    // positions          
    //    -1.0f,  1.0f, -1.0f,
    //    -1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,
    //     1.0f,  1.0f, -1.0f,
    //    -1.0f,  1.0f, -1.0f,

    //    -1.0f, -1.0f,  1.0f,
    //    -1.0f, -1.0f, -1.0f,
    //    -1.0f,  1.0f, -1.0f,
    //    -1.0f,  1.0f, -1.0f,
    //    -1.0f,  1.0f,  1.0f,
    //    -1.0f, -1.0f,  1.0f,

    //     1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,

    //    -1.0f, -1.0f,  1.0f,
    //    -1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f, -1.0f,  1.0f,
    //    -1.0f, -1.0f,  1.0f,

    //    -1.0f,  1.0f, -1.0f,
    //     1.0f,  1.0f, -1.0f,
    //     1.0f,  1.0f,  1.0f,
    //     1.0f,  1.0f,  1.0f,
    //    -1.0f,  1.0f,  1.0f,
    //    -1.0f,  1.0f, -1.0f,

    //    -1.0f, -1.0f, -1.0f,
    //    -1.0f, -1.0f,  1.0f,
    //     1.0f, -1.0f, -1.0f,
    //     1.0f, -1.0f, -1.0f,
    //    -1.0f, -1.0f,  1.0f,
    //     1.0f, -1.0f,  1.0f
    //};

    //glGenBuffers(1, &vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    //ENGINE_LOG("Generated VBO %i.", vbo);
}


void ResourceCubemap::CreateVAO()
{
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //glBindVertexArray(0);
}