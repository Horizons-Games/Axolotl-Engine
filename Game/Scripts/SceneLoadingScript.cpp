#include "StdAfx.h"

#include "SceneLoadingScript.h"

#include "Components/ComponentScript.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Modules/ModuleScene.h"

#include "Defines/ExtensionDefines.h"

REGISTERCLASS(SceneLoadingScript);

namespace
{
constexpr const char* loadingScreenScene = "Scenes\\LoadingScreen" SCENE_EXTENSION;
}

SceneLoadingScript::SceneLoadingScript() : Script()
{
	REGISTER_FIELD(sceneToLoad, std::string);
}

void SceneLoadingScript::Init()
{
#ifdef ENGINE
	Assert(false, "Cannot change scenes while playing in the engine");
#endif // ENGINE
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	Assert(fileSystem->Exists(loadingScreenScene),
		   axo::Format("The default loading screen {} does not exist", loadingScreenScene));
	Assert(fileSystem->Exists(sceneToLoad.c_str()),
		   axo::Format("The scene to load asynchronousy {} does not exist", sceneToLoad));

}

void SceneLoadingScript::StartLoad() const
{
	App->GetModule<ModuleScene>()->LoadSceneAsync(loadingScreenScene,
												  std::bind(&SceneLoadingScript::OnLoadingScreenLoaded, this));
}

void SceneLoadingScript::OnLoadingScreenLoaded() const
{
	App->GetModule<ModuleScene>()->LoadSceneAsync(sceneToLoad,
												  [&]()
												  {
													  LOG_INFO("Scene {} loaded!!", sceneToLoad);
												  });
}
