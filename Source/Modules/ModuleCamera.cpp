#pragma warning (disable: 26495)

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBoxes.h"
#include "Components/ComponentMeshRenderer.h"

#include "Resources/ResourceMesh.h"

#include "Windows/EditorWindows/WindowScene.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"
#include "Geometry/Sphere.h"
#include "Geometry/Triangle.h"

#ifdef ENGINE
#include "Camera/CameraEngine.h"
#endif // ENGINE
#include "Camera/CameraGod.h"



ModuleCamera::ModuleCamera() {};

ModuleCamera::~ModuleCamera() {
};

bool ModuleCamera::Init()
{
	#ifdef ENGINE
		camera = std::make_unique <CameraEngine>();
	#else // ENGINE
		camera = std::make_unique <CameraGod>();
	#endif // GAMEMODE

	camera->Init();
	return true;
}

bool ModuleCamera::Start()
{
	camera->Start();

	return true;
}

update_status ModuleCamera::Update()
{
	camera->Update();
	return UPDATE_CONTINUE;
}
