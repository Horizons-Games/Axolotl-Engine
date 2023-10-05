#include "StdAfx.h"

#include "ComponentSkybox.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "Filesystem/ModuleResources.h"
#include "Resources/ResourceSkyBox.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "DataModels/Program/Program.h"
#include "Cubemap/Cubemap.h"

#include "Camera/Camera.h"

ComponentSkybox::ComponentSkybox(bool active, GameObject* owner) : 
	Component(ComponentType::SKYBOX, active, owner, true), useCubemap(false)
{
}

ComponentSkybox::~ComponentSkybox()
{
}

void ComponentSkybox::Draw(float4x4 view, float4x4 proj) const
{
	if (IsEnabled())
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

			program->BindUniformFloat4x4("view", (const float*) &view, GL_TRUE);
			program->BindUniformFloat4x4("proj", (const float*) &proj, GL_TRUE);

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
	jsonSkybox["skyboxUID"] = skyboxRes->GetUID();
	jsonSkybox["skyboxAssetPath"] = skyboxRes->GetAssetsPath().c_str();
}

void ComponentSkybox::InternalLoad(const Json& meta)
{
	Json jsonSkybox = meta["Skybox"];
	useCubemap = jsonSkybox["useCubemap"];
	UID resUID = jsonSkybox["skyboxUID"];
	std::string resPath = jsonSkybox["skyboxAssetPath"];

#ifdef ENGINE
	skyboxRes = App->GetModule<ModuleResources>()->RequestResource<ResourceSkyBox>(resPath);
#else
	skyboxRes = App->GetModule<ModuleResources>()->SearchResource<ResourceSkyBox>(resUID);
#endif // ENGINE
}

std::shared_ptr<ResourceSkyBox> ComponentSkybox::GetSkyboxResource() const
{
	return skyboxRes;
}

void ComponentSkybox::SetSkyboxResource(std::shared_ptr<ResourceSkyBox> resource)
{
	skyboxRes = resource;
	cubemap.release();
}

Cubemap* ComponentSkybox::GetCubemap()
{
	if (cubemap == nullptr)
	{
		if (!skyboxRes->IsLoaded())
		{
			skyboxRes->Load();
		}
		cubemap = std::make_unique<Cubemap>(skyboxRes->GetGlTexture());
	}
	return cubemap.get();
}
