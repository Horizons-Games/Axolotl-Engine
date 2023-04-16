#include "ComponentImage.h"
#include "ComponentTransform2D.h"
#include "ComponentCanvas.h"
#include "GameObject/GameObject.h"

#include "GL/glew.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"

#include "DataModels/Program/Program.h"
#include "ComponentButton.h"
#include "Resources/ResourceTexture.h"
#include "Resources/ResourceMesh.h"
#include "FileSystem/Json.h"

#include "Windows/EditorWindows/WindowScene.h"

ComponentImage::ComponentImage(bool active, GameObject* owner)
	: Component(ComponentType::IMAGE, active, owner, true), color(float3(1.0f, 1.0f, 1.0f))
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
	if(program)
	{

		program->Activate();
		std::pair<int,int> region = App->editor->GetAvailableRegion();
		
		ComponentTransform2D* transform = static_cast<ComponentTransform2D*>(GetOwner()
			->GetComponent(ComponentType::TRANSFORM2D));

		const float4x4& proj = float4x4::D3DOrthoProjLH(-1, 1, region.first, region.second);
		const float4x4& model = transform->GetGlobalScaledMatrix();
		float4x4 view = float4x4::identity;

		ComponentCanvas* canvas = transform->WhichCanvasContainsMe();
		if(canvas)
		{
			canvas->RecalculateSizeAndScreenFactor();
			float factor = canvas->GetScreenFactor();
			view = view * float4x4::Scale(factor, factor, factor);
		}

		glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&view);
		glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&model);
		glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

		glBindVertexArray(vao);

		glActiveTexture(GL_TEXTURE0);
		program->BindUniformFloat3("spriteColor", GetFullColor());
		if (image) 
		{
			image->Load();
			glBindTexture(GL_TEXTURE_2D, image->GetGlTexture());
			program->BindUniformInt("hasDiffuse", 1);
		}
		else 
		{
			program->BindUniformInt("hasDiffuse", 0);
		}

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

	meta["color_x"] = (float)color.x;
	meta["color_y"] = (float)color.y;
	meta["color_z"] = (float)color.z;
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

	color.x = (float)meta["color_x"];
	color.y = (float)meta["color_y"];
	color.z = (float)meta["color_z"];
}

inline float3 ComponentImage::GetFullColor()
{
	ComponentButton* button = static_cast<ComponentButton*>(GetOwner()->GetComponent(ComponentType::BUTTON));
	if(button != nullptr)
	{
		if (button->IsClicked()) return button->GetColorClicked();
		if (button->IsHovered()) return button->GetColorHovered();
	}
	return color;
}

void ComponentImage::LoadVBO()
{
	float vertices[] = {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
}

void ComponentImage::CreateVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}