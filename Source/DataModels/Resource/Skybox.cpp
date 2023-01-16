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


Skybox::Skybox(const std::weak_ptr<ResourceSkyBox>& skyboxRes)
{
    this->skyboxRes = skyboxRes;
    std::shared_ptr<ResourceSkyBox> skyboxAsShared = this->skyboxRes.lock();
    skyboxAsShared->Load();
}

void Skybox::Draw()
{
    /*glDepthMask(GL_FALSE);

    Program* program = App->program->GetSkyboxProgram();

    program->Activate();
    program->BindUniformFloat4x4("view", (const float*)&App->engineCamera->GetViewMatrix(), GL_TRUE);
    program->BindUniformFloat4x4("proj", (const float*)&App->engineCamera->GetProjectionMatrix(), GL_TRUE);


    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    program->Deactivate();
    glDepthMask(GL_TRUE);*/
}

//void Skybox::LoadOptions(Json& meta)
//{
//    
//    
//    //owner = (GameObject*) meta["owner"];
//
//    //UID uidMesh = meta["meshUID"];
//
//    //SetMesh(std::dynamic_pointer_cast<ResourceMesh>(App->resources->RequestResource(uidMesh).lock()));
//}

