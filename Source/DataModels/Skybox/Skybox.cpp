#include "Skybox.h"

#include "GL/glew.h"

#include "Application.h"
#include "DataModels/Resources/ResourceSkyBox.h"

#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "DataModels/Program/Program.h"

Skybox::Skybox(const std::shared_ptr<ResourceSkyBox>& skyboxRes) : skyboxRes(skyboxRes)
{
    this->skyboxRes->Load();
}

Skybox::~Skybox()
{
}

void Skybox::Draw()
{

    std::shared_ptr<Program> program = App->program->GetProgram(ProgramType::SKYBOX);
    if (program && skyboxRes)
    {
        skyboxRes->Load();
        glDepthMask(GL_FALSE);

        program->Activate();

        program->BindUniformFloat4x4("view", (const float*)&App->engineCamera->GetCamera()->GetViewMatrix(), GL_TRUE);
        program->BindUniformFloat4x4("proj", (const float*)&App->engineCamera->GetCamera()->GetProjectionMatrix(), GL_TRUE);

        glBindVertexArray(skyboxRes->GetVAO());
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxRes->GetGlTexture());

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        program->Deactivate();
        glDepthMask(GL_TRUE);
    }
}