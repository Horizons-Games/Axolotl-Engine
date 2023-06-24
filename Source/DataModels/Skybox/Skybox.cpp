#include "Skybox.h"

#include "GL/glew.h"

#include "Application.h"
#include "DataModels/Resources/ResourceSkyBox.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Program/Program.h"
#include "FileSystem/Json.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"

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
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SKYBOX);
	if (program && skyboxRes)
	{
		if (!skyboxRes->IsLoaded())
		{
			skyboxRes->Load();
		}
		glDepthRange(0.999, 1.0);
		glDepthMask(GL_FALSE);

		program->Activate();
		ModuleCamera* camera = App->GetModule<ModuleCamera>();

		program->BindUniformFloat4x4("view", (const float*) &camera->GetCamera()->GetViewMatrix(), GL_TRUE);
		program->BindUniformFloat4x4("proj", (const float*) &camera->GetCamera()->GetProjectionMatrix(), GL_TRUE);

		glBindVertexArray(skyboxRes->GetVAO());
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxRes->GetGlTexture());

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		program->Deactivate();
		glDepthMask(GL_TRUE);
		glDepthRange(0.0, 1.0);
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
	skyboxRes = App->GetModule<ModuleResources>()->RequestResource<ResourceSkyBox>(resPath);
#else
	skyboxRes = App->GetModule<ModuleResources>()->SearchResource<ResourceSkyBox>(resUID);
#endif // ENGINE
}
