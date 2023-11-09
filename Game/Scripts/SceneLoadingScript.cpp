#include "StdAfx.h"

#include "SceneLoadingScript.h"

#include "Components/ComponentScript.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Modules/ModuleScene.h"

#include "Defines/ExtensionDefines.h"
#include "Defines/FileSystemDefines.h"

REGISTERCLASS(SceneLoadingScript);

SceneLoadingScript::SceneLoadingScript() : Script()
{
	REGISTER_FIELD(sceneToLoad, std::string);
	REGISTER_FIELD(loadingScreenScene, std::string);
}

void SceneLoadingScript::Init()
{
#ifdef ENGINE
	Assert(false, "Cannot change scenes while playing in the engine");
#endif // ENGINE
	// set the proper path here
	sceneToLoad = "Lib/Scenes/" + sceneToLoad + SCENE_EXTENSION;
	loadingScreenScene = "Lib/Scenes/" + loadingScreenScene + SCENE_EXTENSION;

	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	Assert(fileSystem->Exists(sceneToLoad.c_str()),
		axo::Format("The scene to load asynchronousy {} does not exist", sceneToLoad));
	Assert(fileSystem->Exists(loadingScreenScene.c_str()),
		axo::Format("The default loading screen {} does not exist", loadingScreenScene));
}

void SceneLoadingScript::StartLoad() const
{
	// if the script is set through the UI, we cannot check this, so check manually here
	// can't think of a way to automatize this because the member access operator overload (->) can't be virtual :/
	if (container->HasFailed())
	{
		throw AccessingFailedScriptException(axo::Format("Calling SceneLoadingScript after the script raised an exception!"));
	}
	App->GetModule<ModuleScene>()->LoadSceneAsync(loadingScreenScene,
		std::bind(&SceneLoadingScript::OnLoadingScreenLoaded, this));
}

void SceneLoadingScript::OnLoadingScreenLoaded() const
{
	App->ScheduleTask(
		[this]()
		{
			App->GetModule<ModuleScene>()->LoadSceneAsync(sceneToLoad,
				[&]()
				{
					LOG_INFO("Scene {} loaded!!", sceneToLoad);
				});
		},
		6U /*Wait 6 frames to start the next load, so Render can be updated and the scene shown*/);
}
