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
	image = App->resources->RequestResource<ResourceTexture>("Assets/Skybox/skybox.sky");
	static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
	};
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
}

ComponentImage::~ComponentImage()
{
}

void ComponentImage::Update()
{
}

void ComponentImage::Draw()
{
	Program* program = App->program->GetProgram(ProgramType::SKYBOX);
	if(program && image)
	{
		image->Load();
		glDepthMask(GL_FALSE);

		program->Activate();

		program->BindUniformFloat4x4("view", (const float*)&App->camera->GetCamera()->GetViewMatrix(), GL_TRUE);
		program->BindUniformFloat4x4("proj", (const float*)&App->camera->GetCamera()->GetProjectionMatrix(), GL_TRUE);

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		//glActiveTexture(GL_TEXTURE0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
		program->Deactivate();
		glDepthMask(GL_TRUE);
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
