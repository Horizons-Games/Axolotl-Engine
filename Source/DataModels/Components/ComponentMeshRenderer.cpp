#pragma warning (disable: 26495)

#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"
#include "Program/Program.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceMesh.h"

#include "GameObject/GameObject.h"

#include "GL/glew.h"

#ifdef ENGINE
#include "DataModels/Resources/EditorResource/EditorResourceInterface.h"
#endif // ENGINE

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESHRENDERER, active, owner, true)
{
}

ComponentMeshRenderer::ComponentMeshRenderer(const ComponentMeshRenderer& componentMeshRenderer):
	Component(componentMeshRenderer), mesh(componentMeshRenderer.GetMesh())
{
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
	if (mesh)
		mesh->Unload();
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::Draw()
{
	//this should be in an EditorComponent class, or something of the like
	//but for now have it here
#ifdef ENGINE
	if (mesh && std::dynamic_pointer_cast<EditorResourceInterface>(mesh)->ToDelete())
	{
		mesh = nullptr;
	}
#endif // ENGINE

	if (this->IsMeshLoaded()) //pointer not empty
	{
		if (!mesh->IsLoaded())
		{
			mesh->Load();
		}

		Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

		if (program)
		{
			program->Activate();
			const float4x4& view = App->camera->GetCamera()->GetViewMatrix();
			const float4x4& proj = App->camera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
			glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
			glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

			glBindVertexArray(mesh->GetVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

			glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			program->Deactivate();
		}
	}
}

void ComponentMeshRenderer::DrawHighlight()
{
	if (IsMeshLoaded()) //pointer not empty
	{
		if (!mesh->IsLoaded())
		{
			mesh->Load();
		}

		float scale = 10.1f;
		Program* program = App->program->GetProgram(ProgramType::HIGHLIGHT);

		if (program)
		{
			program->Activate();
			const float4x4& view = App->camera->GetCamera()->GetViewMatrix();
			const float4x4& proj = App->camera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
			GLint programInUse;
			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

			glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
			glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
			glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

			glBindVertexArray(mesh->GetVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

			glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			program->Deactivate();
		}
	}
}

void ComponentMeshRenderer::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidMesh = 0;
	std::string assetPath = "";

	if(mesh)
	{
		uidMesh = mesh->GetUID();
		assetPath = mesh->GetAssetsPath();
	}

	meta["meshUID"] = (UID)uidMesh;
	meta["assetPathMesh"] = assetPath.c_str();
}

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

#ifdef ENGINE
	std::string path = meta["assetPathMesh"];
	bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceMesh> resourceMesh = App->resources->RequestResource<ResourceMesh>(path);
		if (resourceMesh)
		{
			SetMesh(resourceMesh);
		}
	}
#else
	UID uidMesh = meta["meshUID"];
	std::shared_ptr<ResourceMesh> resourceMesh = App->resources->SearchResource<ResourceMesh>(uidMesh);
	if (resourceMesh)
	{
		SetMesh(resourceMesh);
	}
#endif
}

void ComponentMeshRenderer::SetMesh(const std::shared_ptr<ResourceMesh>& newMesh)
{
	mesh = newMesh;

	if (mesh)
	{
		mesh->Load();
		GetOwner()->Encapsule(mesh->GetVertices().data(), mesh->GetNumVertices());
	}
}