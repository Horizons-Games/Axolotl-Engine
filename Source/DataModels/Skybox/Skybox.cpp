#include "Skybox.h"

#include "GL/glew.h"

#include "Application.h"
#include "DataModels/Resources/ResourceSkyBox.h"

#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "DataModels/Program/Program.h"

Skybox::Skybox(const std::weak_ptr<ResourceSkyBox>& skyboxRes) : skyboxRes(skyboxRes),
    skyboxUID(0ULL)
{
    std::shared_ptr<ResourceSkyBox> skyboxAsShared = this->skyboxRes.lock();
    if (skyboxAsShared)
    {
        skyboxAsShared->Load();
    }
}

Skybox::~Skybox()
{
}

void Skybox::Draw()
{
    glDepthMask(GL_FALSE);

    Program* program = App->program->GetProgram(ProgramType::SKYBOX);
    if (program) 
    {
        program->Activate();

        program->BindUniformFloat4x4("view", (const float*)&App->engineCamera->GetViewMatrix(), GL_TRUE);
        program->BindUniformFloat4x4("proj", (const float*)&App->engineCamera->GetProjectionMatrix(), GL_TRUE);

        std::shared_ptr<ResourceSkyBox> skyboxAsShared = this->skyboxRes.lock();
        if (skyboxAsShared)
        {
            glBindVertexArray(skyboxAsShared->GetVAO());
            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxAsShared->GetGlTexture());

            glDrawArrays(GL_TRIANGLES, 0, 36);

            glBindVertexArray(0);
            program->Deactivate();
            glDepthMask(GL_TRUE);
        }
    }
}