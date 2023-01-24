#pragma warning (disable: 26495)

#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"
#include "ComponentBoundingBoxes.h"

#include "Application.h"

#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"

#include "GameObject/GameObject.h"

#include "GL/glew.h"

#include "Windows/EditorWindows/ImporterWindows/WindowMeshInput.h"

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, const std::shared_ptr<GameObject>& owner)
	: Component(ComponentType::MESHRENDERER, active, owner, true)
{
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (meshAsShared)
		mesh.lock()->Unload();
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::Draw()
{
	//lock it so it does not expire during this block
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	if (meshAsShared) //pointer not empty
	{
		if (!meshAsShared->IsLoaded())
		{
			meshAsShared->Load();
		}

		unsigned program = App->program->GetProgram();
		const float4x4& view = App->engineCamera->GetViewMatrix();
		const float4x4& proj = App->engineCamera->GetProjectionMatrix();
		const float4x4& model =
			std::static_pointer_cast<ComponentTransform>(GetOwner().lock()
				->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

		GLint programInUse;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

		if (program != programInUse)
		{
			glUseProgram(program);
		}

		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_TRUE, (const float*)&proj);

		glBindVertexArray(meshAsShared->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshAsShared->GetEBO());

		glDrawElements(GL_TRIANGLES, meshAsShared->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void ComponentMeshRenderer::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();

	UID uidMesh = 0;
	std::string assetPath = "";

	if(meshAsShared)
	{
		uidMesh = meshAsShared->GetUID();
		assetPath = meshAsShared->GetAssetsPath();
	}

	meta["meshUID"] = (UID)uidMesh;
	meta["assetPathMesh"] = assetPath.c_str();

	//meta["mesh"] = (std::weak_ptr<ResourceMesh>) mesh;
}

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	UID uidMesh = meta["meshUID"];
	std::shared_ptr<ResourceMesh> resourceMesh = App->resources->RequestResource<ResourceMesh>(uidMesh).lock();

	if (resourceMesh)
	{
		SetMesh(resourceMesh);
	}
	else
	{
		std::string path = meta["assetPathMesh"];
		bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
		if (resourceExists) 
		{
			uidMesh = App->resources->ImportResource(path);
			resourceMesh = App->resources->RequestResource<ResourceMesh>(uidMesh).lock();
			SetMesh(resourceMesh);
		}
	}
}

void ComponentMeshRenderer::SetMesh(const std::weak_ptr<ResourceMesh>& newMesh)
{
	mesh = newMesh;
	std::shared_ptr<ResourceMesh> meshAsShared = mesh.lock();


	if (meshAsShared)
	{
		meshAsShared->Load();
		std::shared_ptr<ComponentBoundingBoxes> boundingBox =
			std::static_pointer_cast<ComponentBoundingBoxes>(GetOwner().lock()->GetComponent(ComponentType::BOUNDINGBOX));
		boundingBox->Encapsule(meshAsShared->GetVertices().data(), meshAsShared->GetNumVertices());
	}
}