#pragma once
#include "IObject.h"
class GameObject;
class Application;

class IScript : public IObject
{
	public:
		virtual void Init() = 0;
		virtual void Start() = 0;
		virtual void SetGameObject(GameObject* owner) = 0;
		virtual void SetApplication(Application* app) = 0;
		virtual void Update(float deltaTime) = 0;
	protected:
		GameObject *owner;
		Application* App;
};