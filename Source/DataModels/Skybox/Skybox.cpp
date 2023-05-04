#include "Skybox.h"

#include "GL/glew.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceSkyBox.h"

#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "DataModels/Program/Program.h"
#include "FileSystem/Json.h"

Skybox::Skybox() : skyboxRes(nullptr)
{
}

Skybox::Skybox(const std::shared_ptr<ResourceSkyBox>& skyboxRes) : skyboxRes(skyboxRes)
{
}

Skybox::~Skybox()
{
}

void Skybox::Draw() const
{

    Program* program = App->program->GetProgram(ProgramType::SKYBOX);
    if (program && skyboxRes) 
    {
        if (!skyboxRes->IsLoaded())
        {
            skyboxRes->Load();
        }
        glDepthMask(GL_FALSE);

        program->Activate();

        program->BindUniformFloat4x4("view", (const float*)&App->camera->GetCamera()->GetViewMatrix(), GL_TRUE);
        program->BindUniformFloat4x4("proj", (const float*)&App->camera->GetCamera()->GetProjectionMatrix(), GL_TRUE);

        glBindVertexArray(skyboxRes->GetVAO());
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxRes->GetGlTexture());

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        program->Deactivate();
        glDepthMask(GL_TRUE);
    }
}

void Skybox::SaveOptions(Json& json) const
{
    Json jsonSkybox = json["Skybox"];

    jsonSkybox["skyboxUID"] = skyboxRes->GetUID();
    jsonSkybox["skyboxAssetPath"] = skyboxRes->GetAssetsPath().c_str();
}

void Skybox::LoadOptions(Json& json)
{
    Json jsonSkybox = json["Skybox"];

    UID resUID = jsonSkybox["skyboxUID"];
    std::string resPath = jsonSkybox["skyboxAssetPath"];

#ifdef ENGINE
    skyboxRes = App->resources->RequestResource<ResourceSkyBox>(resPath);
#else
    skyboxRes = App->resources->SearchResource<ResourceSkyBox>(resUID);
#endif // ENGINE
}
