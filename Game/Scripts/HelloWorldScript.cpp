#include "HelloWorldScript.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(HelloWorldScript);

HelloWorldScript::HelloWorldScript() : Script(), helloWorld("Hello World"), movingGameObject(nullptr)
{
	REGISTER_FIELD(HelloWorld, std::string);
	REGISTER_FIELD(MovingGameObject, GameObject*);
}

void HelloWorldScript::Update(float deltaTime)
{
	// Print "Hello World" each frame
	ENGINE_LOG("%s", helloWorld.c_str());

	// Recieves a GameObject and moves it (sometimes the transform does not update until you click on it)
	if (movingGameObject != nullptr)
	{
		ComponentTransform* goTransform = static_cast<ComponentTransform*>
			(movingGameObject->GetComponent(ComponentType::TRANSFORM));

		goTransform->SetPosition(float3(goTransform->GetPosition().x,
										goTransform->GetPosition().y + 0.1f, 
										goTransform->GetPosition().z));
	}

	// When pressing a key, a new GameObject is created
	GameObject* sceneRoot = App->scene->GetLoadedScene()->GetRoot();

	// If you press P 3 times, the engine explodes, but that has nothing to do with Scripting (IMO)
	if (App->input->GetKey(SDL_SCANCODE_P) == KeyState::DOWN)
	{
		App->scene->GetLoadedScene()->Create3DGameObject("Empty Capsule", sceneRoot, Premade3D::CAPSULE);
	}
}

std::string HelloWorldScript::GetHelloWorld() const
{
	return helloWorld;
}

void HelloWorldScript::SetHelloWorld(const std::string& helloWorld)
{
	this->helloWorld = helloWorld;
}

GameObject* HelloWorldScript::GetMovingGameObject() const
{
	return movingGameObject;
}

void HelloWorldScript::SetMovingGameObject(GameObject* movingGameObject)
{
	this->movingGameObject = movingGameObject;
}