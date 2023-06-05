#include "ModuleRenderer.h"

#include "Application.h"
#include "EmitterInstance.h"
#include "ParticleEmitter.h"
#include "ModuleBase.h"

#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "Math/float4x4.h"
#include "Math/float3x3.h"

#include "Modules/ModuleCamera.h"

#include "Program/Program.h"

#include "GL/glew.h"

#include "ImGui/imgui.h"

#include <algorithm>

ModuleRenderer::ModuleRenderer(ParticleEmitter* emitter) : ParticleModule(ModuleType::RENDER, emitter)
{
	alignment = Alignment::WORLD;
	blendingMode = BlendingMode::ALPHA;
	numInstances = 0;

	static const float vertices[] = { -0.5f, -0.5f, 0.0f,
									   0.5f, -0.5f, 0.0f,
									   0.5f,  0.5f, 0.0f,
									  -0.5f,  0.5f, 0.0f };

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	static const unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(4 * sizeof(float3)));

	// Instances
	glGenBuffers(1, &instanceVbo);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);

	unsigned int sizeOfVertex = sizeof(float3) * 4 + sizeof(float4);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)0);
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(sizeof(float3)));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(2 * sizeof(float3)));
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(3 * sizeof(float3)));
	glVertexAttribDivisor(6, 1);

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)(4 * sizeof(float3)));
	glVertexAttribDivisor(7, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ModuleRenderer::~ModuleRenderer()
{
	// Buffer cleanup
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &instanceVbo);
}

void ModuleRenderer::Spawn(EmitterInstance* instance)
{
}

void ModuleRenderer::Update(EmitterInstance* instance)
{
}

void ModuleRenderer::UpdateInstanceBuffer(EmitterInstance* instance)
{
	unsigned int stride = sizeof(float3) * 4 + sizeof(float4);
	unsigned int aliveParticles = instance->GetAliveParticles();

	if (numInstances < aliveParticles)
	{
		numInstances = aliveParticles;
		glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
		glBufferData(GL_ARRAY_BUFFER, stride * numInstances, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
	char* instanceData = static_cast<char*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, stride * numInstances, 
															 GL_MAP_WRITE_BIT));

	const Camera* camera = App->GetModule<ModuleCamera>()->GetSelectedCamera();
	float3 cameraPos = camera->GetPosition();

	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();
	std::vector<unsigned int> sortedPositions = instance->GetSortedPositions();

	for (int i = 0; i < sortedPositions.size(); ++i)
	{
		EmitterInstance::Particle& particle = particles[sortedPositions[i]];

		if (particle.lifespan > 0.0f)
		{
			float3 translation = particle.tranform.TranslatePart();
			float3 xAxis;
			float3 yAxis;
			float3 zAxis;

			switch (alignment)
			{
			case Alignment::SCREEN:
				zAxis = (cameraPos - translation).Normalized();
				yAxis = camera->GetProjectionMatrix().WorldY();
				xAxis = yAxis.Cross(zAxis);
				break;

			case Alignment::WORLD:
				zAxis = (cameraPos - translation).Normalized();
				yAxis = float3::unitY;
				xAxis = yAxis.Cross(zAxis).Normalized();
				break;

			case Alignment::AXIAL:
				yAxis = float3::unitX;
				xAxis = yAxis.Cross(cameraPos - translation).Normalized();
				zAxis = yAxis.Cross(xAxis).Normalized();
				break;
			}

			Quat rotation(zAxis, particle.rotation);
			yAxis = rotation.Transform(yAxis);
			xAxis = rotation.Transform(xAxis);

			xAxis *= particle.size;
			yAxis *= particle.size;
			zAxis *= particle.size;

			particle.tranform.SetCol3(0, xAxis);
			particle.tranform.SetCol3(1, yAxis);
			particle.tranform.SetCol3(2, zAxis);

			float3* instanceInfo = reinterpret_cast<float3*>(instanceData + (i * stride));
			instanceInfo[0] = xAxis;
			instanceInfo[1] = yAxis;
			instanceInfo[2] = zAxis;
			instanceInfo[3] = translation;

			float4* color = reinterpret_cast<float4*>(instanceData + (i * stride) + sizeof(float3) * 4);
			*color = particle.color;
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRenderer::DrawParticles(EmitterInstance* instance)
{
	if (instance->GetAliveParticles() > 0)
	{
		UpdateInstanceBuffer(instance);

		Program* program = instance->GetProgram();

		// Identity 'cause the particles are already in global space
		program->BindUniformFloat4x4(2, reinterpret_cast<const float*>(&float4x4::identity), true);

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);

		switch (blendingMode)
		{
		case BlendingMode::ALPHA:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
			
		case BlendingMode::ADDITIVE:
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		}

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, instance->GetAliveParticles());

		glDisable(GL_BLEND);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void ModuleRenderer::DrawImGui()
{
	if (ImGui::TreeNodeEx("Render Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		if (ImGui::BeginTable("##renderTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Alignment");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			ModuleRenderer::Alignment alignment = GetAlignment();

			const char* alignmentItems[] = { "SCREEN", "WORLD", "AXIAL" };

			static const char* currentItem;
			switch (alignment)
			{
			case ModuleRenderer::Alignment::SCREEN:
				currentItem = alignmentItems[0];
				break;
			case ModuleRenderer::Alignment::WORLD:
				currentItem = alignmentItems[1];
				break;
			case ModuleRenderer::Alignment::AXIAL:
				currentItem = alignmentItems[2];
				break;
			}

			if (ImGui::BeginCombo("##alignmentCombo", currentItem))
			{
				for (int n = 0; n < IM_ARRAYSIZE(alignmentItems); n++)
				{
					bool isSelected = (currentItem == alignmentItems[n]);

					if (ImGui::Selectable(alignmentItems[n], isSelected))
					{
						currentItem = alignmentItems[n];

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				if (currentItem == alignmentItems[0])
				{
					alignment = ModuleRenderer::Alignment::SCREEN;
				}
				else if (currentItem == alignmentItems[1])
				{
					alignment = ModuleRenderer::Alignment::WORLD;
				}
				else if (currentItem == alignmentItems[2])
				{
					alignment = ModuleRenderer::Alignment::AXIAL;
				}

				SetAlignment(alignment);

				ImGui::EndCombo();
			}

			ImGui::TableNextColumn();
			ImGui::Text("Blending");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			ModuleRenderer::BlendingMode blending = GetBlending();

			const char* blendingItems[] = { "ALPHA", "ADDITIVE" };

			switch (blending)
			{
			case ModuleRenderer::BlendingMode::ALPHA:
				currentItem = blendingItems[0];
				break;
			case ModuleRenderer::BlendingMode::ADDITIVE:
				currentItem = blendingItems[1];
				break;
			}

			if (ImGui::BeginCombo("##blendingCombo", currentItem))
			{
				for (int n = 0; n < IM_ARRAYSIZE(blendingItems); n++)
				{
					bool isSelected = (currentItem == blendingItems[n]);

					if (ImGui::Selectable(blendingItems[n], isSelected))
					{
						currentItem = blendingItems[n];

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				if (currentItem == blendingItems[0])
				{
					blending = ModuleRenderer::BlendingMode::ALPHA;
				}
				else if (currentItem == blendingItems[1])
				{
					blending = ModuleRenderer::BlendingMode::ADDITIVE;
				}

				SetBlending(blending);

				ImGui::EndCombo();
			}

			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
