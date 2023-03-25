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
	//provisional TODO
	image = App->resources->RequestResource<ResourceTexture>("Assets/Textures/prop_shield_01_d.png");

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	unsigned int VBO;
	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(this->quadVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	if(program)
	{
		//image->Load();
		//glDepthMask(GL_FALSE);

		program->Activate();

		//program->BindUniformFloat4x4("view", (const float*)&App->camera->GetCamera()->GetViewMatrix(), GL_TRUE);
		//program->BindUniformFloat4x4("proj", (const float*)&App->camera->GetCamera()->GetProjectionMatrix(), GL_TRUE);

		glBindVertexArray(this->quadVAO);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, image->GetGlTexture());

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		program->Deactivate();
		//glDepthMask(GL_TRUE);
	}
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
