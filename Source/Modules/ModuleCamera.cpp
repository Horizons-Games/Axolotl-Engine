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

#include "Camera/CameraEngine.h"

ModuleCamera::ModuleCamera() {};

ModuleCamera::~ModuleCamera() {
};

bool ModuleCamera::Init()
{
	camera = std::make_unique <CameraEngine>();
	camera->Init();
	/*Moved to CameraEngine
	*/
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
	/*Code Moved to CameraEngine*/
	return UPDATE_CONTINUE;
}
