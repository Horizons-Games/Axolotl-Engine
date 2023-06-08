#include "HelloWorldScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(HelloWorldScript);

HelloWorldScript::HelloWorldScript() : Script(), helloWorld("Hello World"), movingGameObject(nullptr)
{
	REGISTER_FIELD(helloWorld, std::string);
	REGISTER_FIELD(movingGameObject, GameObject*);
}

void HelloWorldScript::Update(float deltaTime)
{
	// Print "Hello World" each frame
	LOG_INFO("{}", helloWorld);

	// Receives a GameObject and moves it (sometimes the transform does not update until you click on it)
	if (movingGameObject != nullptr)
	{
		ComponentTransform* goTransform = movingGameObject->GetComponent<ComponentTransform>();

		goTransform->SetPosition(float3(goTransform->GetPosition().x,
			goTransform->GetPosition().y + 0.1f,
			goTransform->GetPosition().z));
	}

	// When pressing a key, a new GameObject is created
	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	GameObject* sceneRoot = loadedScene->GetRoot();

	// If you press P 3 times, the engine explodes, but that has nothing to do with Scripting (IMO)
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_P) == KeyState::DOWN)
	{
		loadedScene->Create3DGameObject("Empty Capsule", sceneRoot, Premade3D::CAPSULE);
	}
}
