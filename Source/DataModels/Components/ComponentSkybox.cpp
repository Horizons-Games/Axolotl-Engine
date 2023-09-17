#include "StdAfx.h"

#include "ComponentSkybox.h"
#include "GameObject/GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "Filesystem/ModuleResources.h"
#include "Scene/Scene.h"
#include "Resources/ResourceSkyBox.h"
#include "DataModels/Program/Program.h"

#include "Cubemap/Cubemap.h"

#include "Camera/Camera.h"

ComponentSkybox::ComponentSkybox(bool active, GameObject* owner) : 
	Component(ComponentType::SKYBOX, active, owner, true), useCubemap(false)
	enable(true)
{
}

ComponentSkybox::~ComponentSkybox()
{
}

void ComponentSkybox::Draw() const
{
	if (enable)
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
}

void ComponentSkybox::InternalSave(Json& meta)
{
	Json jsonSkybox = meta["Skybox"];
	jsonSkybox["useCubemap"] = useCubemap;
	jsonSkybox["enable"] = this->enable;
	jsonSkybox["skyboxUID"] = skyboxRes->GetUID();
	jsonSkybox["skyboxAssetPath"] = skyboxRes->GetAssetsPath().c_str();
}

void ComponentSkybox::InternalLoad(const Json& meta)
{
	Json jsonSkybox = meta["Skybox"];
	useCubemap = jsonSkybox["useCubemap"];
	this->enable = jsonSkybox["enable"];
	UID resUID = jsonSkybox["skyboxUID"];
	std::string resPath = jsonSkybox["skyboxAssetPath"];

#ifdef ENGINE
	skyboxRes = App->GetModule<ModuleResources>()->RequestResource<ResourceSkyBox>(resPath);
#else
	skyboxRes = App->GetModule<ModuleResources>()->SearchResource<ResourceSkyBox>(resUID);
#endif // ENGINE
}

void ComponentSkybox::SignalEnable()
{
	enable = true;
}

void ComponentSkybox::SignalDisable()
{
	enable = false;
}

std::shared_ptr<ResourceSkyBox> ComponentSkybox::GetSkyboxResource() const
{
	cubemap = new Cubemap(skybox->GetSkyboxResource()->GetGlTexture());
	return skyboxRes;
}

void ComponentSkybox::SetSkyboxResource(std::shared_ptr<ResourceSkyBox> resource)
{
	this->skyboxRes = resource;
}

Cubemap* ComponentSkybox::GetCubemap()
{
	return cubemap;
}
