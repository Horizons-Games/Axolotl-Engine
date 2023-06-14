#include "ComponentImage.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "GameObject/GameObject.h"

#include "GL/glew.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"

#include "ComponentButton.h"
#include "DataModels/Program/Program.h"
#include "FileSystem/Json.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceTexture.h"

#include "Windows/EditorWindows/WindowScene.h"

ComponentImage::ComponentImage(bool active, GameObject* owner) :
	Component(ComponentType::IMAGE, active, owner, true),
	color(float4(1.0f, 1.0f, 1.0f, 1.0f))
{
}

ComponentImage::~ComponentImage()
{
}

void ComponentImage::Draw() const
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SPRITE);
	if (program)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		program->Activate();

		ComponentTransform2D* transform = GetOwner()->GetComponent<ComponentTransform2D>();

		const float4x4& proj = App->GetModule<ModuleCamera>()->GetOrthoProjectionMatrix();
		const float4x4& model = transform->GetGlobalScaledMatrix();
		float4x4 view = float4x4::identity;

		ComponentCanvas* canvas = transform->WhichCanvasContainsMe();
		if (canvas)
		{
			canvas->RecalculateSizeAndScreenFactor();
			float factor = canvas->GetScreenFactor();
			view = view * float4x4::Scale(factor, factor, factor);
		}

		glUniformMatrix4fv(2, 1, GL_TRUE, (const float*) &view);
		glUniformMatrix4fv(1, 1, GL_TRUE, (const float*) &model);
		glUniformMatrix4fv(0, 1, GL_TRUE, (const float*) &proj);

		glBindVertexArray(App->GetModule<ModuleUI>()->GetQuadVAO());

		glActiveTexture(GL_TEXTURE0);
		program->BindUniformFloat4("spriteColor", GetFullColor());
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

		glDisable(GL_BLEND);
		program->Deactivate();
	}
}

void ComponentImage::InternalSave(Json& meta)
{
	UID uidImage = 0;
	std::string assetPath = "";

	if (image)
	{
		uidImage = image->GetUID();
		assetPath = image->GetAssetsPath();
	}
	meta["imageUID"] = static_cast<UID>(uidImage);
	meta["assetPathImage"] = assetPath.c_str();

	meta["color_x"] = static_cast<float>(color.x);
	meta["color_y"] = static_cast<float>(color.y);
	meta["color_z"] = static_cast<float>(color.z);
	meta["color_w"] = static_cast<float>(color.w);
}

void ComponentImage::InternalLoad(const Json& meta)
{
#ifdef ENGINE
	std::string path = meta["assetPathImage"];
	bool resourceExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceTexture> resourceImage =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(path);
		if (resourceImage)
		{
			image = resourceImage;
		}
	}
#else
	UID uidImage = meta["imageUID"];
	std::shared_ptr<ResourceTexture> resourceImage =
		App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(uidImage);
	if (resourceImage)
	{
		image = resourceImage;
	}
#endif

	color.x = static_cast<float>(meta["color_x"]);
	color.y = static_cast<float>(meta["color_y"]);
	color.z = static_cast<float>(meta["color_z"]);
	color.w = static_cast<float>(meta["color_w"]);
}

inline float4 ComponentImage::GetFullColor() const
{
	ComponentButton* button = GetOwner()->GetComponent<ComponentButton>();
	if (button != nullptr)
	{
		if (button->IsClicked())
			return button->GetColorClicked();
		if (button->IsHovered())
			return button->GetColorHovered();
	}
	return color;
}