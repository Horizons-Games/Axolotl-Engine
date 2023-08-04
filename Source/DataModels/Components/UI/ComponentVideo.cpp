#include "StdAfx.h"

#include "GameObject/GameObject.h"
#include "GL/glew.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleUI.h"
#include "DataModels/Program/Program.h"
#include "ComponentVideo.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "GameObject/GameObject.h"

ComponentVideo::ComponentVideo(bool active, GameObject* owner) :
	Component(ComponentType::VIDEO, active, owner, true)
{
}

ComponentVideo::~ComponentVideo()
{
}

void ComponentVideo::Draw() const
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SPRITE);
	if (program)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		program->Activate();

		ComponentTransform2D* transform = GetOwner()->GetComponentInternal<ComponentTransform2D>();

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
		program->BindUniformFloat4("spriteColor", float4(1.0f, 1.0f, 1.0f, 1.0f));
		program->BindUniformFloat("renderPercentage", 1.0f);
		program->BindUniformInt("direction", 0);
		program->BindUniformInt("hasDiffuse", 0);
		/*if (image)
		{
			image->Load();
			glBindTexture(GL_TEXTURE_2D, image->GetGlTexture());
			program->BindUniformInt("hasDiffuse", 1);
		}
		else
		{
			program->BindUniformInt("hasDiffuse", 0);
		}*/

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisable(GL_BLEND);
		program->Deactivate();
	}
}

void ComponentVideo::InternalSave(Json& meta)
{
}

void ComponentVideo::InternalLoad(const Json& meta)
{
}
