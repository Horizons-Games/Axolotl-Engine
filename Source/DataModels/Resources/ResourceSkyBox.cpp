#include "ResourceSkyBox.h"

#include "GL/glew.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceTexture.h"

void ResourceSkyBox::InternalLoad()
{
    glGenTextures(1, &glTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glTexture);

    for (int i = 0; i < texturesUIDs.size(); ++i)
    {
        std::shared_ptr<ResourceTexture> textI =
            std::dynamic_pointer_cast<ResourceTexture>(App->resources->RequestResource(texturesUIDs[i]).lock());

        if (textI)
        {
            textI->Load();
            std::vector<uint8_t> aux = textI->GetPixels();
            if (i != 2 && i != 3)
            {
                std::reverse(aux.begin(), aux.end());
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, textI->GetInternalFormat(), textI->GetWidth(),
                    textI->GetHeight(), 0, GL_ABGR_EXT, textI->GetImageType(), &(aux[0]));
            }
            else
            {
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
}

void ResourceSkyBox::InternalUnload()
{
    //this will keep the capacity to 6
    texturesUIDs.clear();
    glDeleteTextures(1, &glTexture);
    glTexture = 0;
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