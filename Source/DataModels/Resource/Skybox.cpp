#include "Skybox.h"

#include <vector>
#include <string>

#include "Application.h"
#include "ModuleTexture.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include <DataModels/Resources/ResourceSkyBox.h>

//#include "ModuleProgram.h"
//#include "ModuleEngineCamera.h"
//#include "Program.h"


Skybox::Skybox()
{

    this->skyboxRes = std::static_pointer_cast<ResourceSkyBox>(App->resources->RequestResource(skyboxUID).lock());

    //texture = App->textures->LoadCubeMap(faces);

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
}

void Skybox::Draw()
{
    //glDepthMask(GL_FALSE);

    ////Program* program = App->program->GetSkyboxProgram();

    ////program->Activate();
    ////program->BindUniformFloat4x4("view", (const float*)&App->engineCamera->GetViewMatrix(), GL_TRUE);
    ////program->BindUniformFloat4x4("proj", (const float*)&App->engineCamera->GetProjectionMatrix(), GL_TRUE);


    //glBindVertexArray(vao);
    //glActiveTexture(GL_TEXTURE0);

    //glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    //glDrawArrays(GL_TRIANGLES, 0, 36);

    //glBindVertexArray(0);
    ////program->Deactivate();
    //glDepthMask(GL_TRUE);
}

