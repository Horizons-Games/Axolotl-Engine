#include "ComponentImage.h"
#include "ComponentTransform2D.h"
#include "GameObject/GameObject.h"

#include "GL/glew.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleProgram.h"

#include "DataModels/Program/Program.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceMesh.h"
#include "FileSystem/Json.h"

ComponentImage::ComponentImage(bool active, GameObject* owner)
	: Component(ComponentType::IMAGE, active, owner, true)
{
	//provisional TODO
	LoadVBO();
	CreateVAO();
}

ComponentImage::~ComponentImage()
{
}

void ComponentImage::Update()
{
}

void ComponentImage::Draw()
{
	Program* program = App->program->GetProgram(ProgramType::SPRITE);
	if(program && image)
	{
		image->Load();
		program->Activate();
		const float4x4& model =
				static_cast<ComponentTransform2D*>(GetOwner()
					->GetComponent(ComponentType::TRANSFORM2D))->GetLocalMatrix();
		glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&model);

		glBindVertexArray(vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image->GetGlTexture());

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		program->Deactivate();
	}
}

void ComponentImage::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidImage = 0;
	std::string assetPath = "";

	if (image)
	{
		uidImage = image->GetUID();
		assetPath = image->GetAssetsPath();
	}
	meta["imageUID"] = (UID)uidImage;
	meta["assetPathImage"] = assetPath.c_str();
}

void ComponentImage::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

#ifdef ENGINE
	std::string path = meta["assetPathImage"];
	bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceTexture> resourceImage = App->resources->RequestResource<ResourceTexture>(path);
		if (resourceImage)
		{
			image = resourceImage;
		}
	}
#else
	UID uidImage = meta["imageUID"];
	std::shared_ptr<ResourceTexture> resourceImage = App->resources->SearchResource<ResourceTexture>(uidImage);
	if (resourceImage)
	{
		image = resourceImage;
	}
#endif
}

void ComponentImage::LoadVBO()
{
	float skyboxVertices[] = {
		// positions          
		-0.5,  0.5, 0.0f, 1.0f,
		-0.5, -0.5, 0.0f, 0.0f,
		 0.5, -0.5, 1.0f, 0.0f,
		 0.5, -0.5, 1.0f, 0.0f,
		 0.5,  0.5, 1.0f, 1.0f,
		-0.5,  0.5, 0.0f, 1.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
}

void ComponentImage::CreateVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}