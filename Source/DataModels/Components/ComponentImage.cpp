#include "ComponentImage.h"

#include "GL/glew.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleProgram.h"

#include "DataModels/Program/Program.h"
#include "Resources/ResourceTexture.h"
#include "FileSystem/Json.h"

ComponentImage::ComponentImage(bool active, GameObject* owner)
	: Component(ComponentType::IMAGE, active, owner, true)
{
}

ComponentImage::~ComponentImage()
{
}

void ComponentImage::Update()
{
}

void ComponentImage::Draw()
{
}

void ComponentImage::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentImage::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}
