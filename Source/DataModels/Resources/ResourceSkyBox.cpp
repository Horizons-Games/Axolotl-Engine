#include "ResourceSkyBox.h"

#include "GL/glew.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceTexture.h"

ResourceSkyBox::ResourceSkyBox(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
    const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath),
    textures(6), vbo(0), vao(0)
{
}

ResourceSkyBox::~ResourceSkyBox()
{
    Unload();
}

void ResourceSkyBox::InternalLoad()
{
    glGenTextures(1, &glTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture);

    for (int i = 0; i < textures.size(); ++i)
    {
        std::shared_ptr<ResourceTexture> textI = textures[i];

        if (textI)
        {
            textI->Load();
            std::vector<uint8_t> aux = textI->GetPixels();
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textI->GetInternalFormat(), textI->GetWidth(),
                textI->GetHeight(), 0, textI->GetFormat(), textI->GetImageType(), &(aux[0]));
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    LoadVBO();
    CreateVAO();
}

void ResourceSkyBox::InternalUnload()
{
    //this will keep the capacity to 6
    textures.clear();
    glDeleteTextures(1, &glTexture);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glTexture = 0;
    vbo = 0;
    vao = 0;
}

bool ResourceSkyBox::ChildChanged() const
{
    bool result = false;
    for (std::shared_ptr<ResourceTexture> texture : textures)
    {
        if (texture && texture->IsChanged())
        {
            result = true;
            texture->SetChanged(false);
        }
    }
    return result;
}


void ResourceSkyBox::LoadVBO()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    ENGINE_LOG("Generated VBO %i.", vbo);
}


void ResourceSkyBox::CreateVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}