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

ModuleRenderer::ModuleRenderer(ParticleEmitter* emitter) : ParticleModule(ModuleType::RENDER, emitter)
{
	alignment = Alignment::WORLD;
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

	unsigned int sizeOfVertex = sizeof(float3) * 4;

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
	unsigned int stride = sizeof(float3) * 4;
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

	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	float3 cameraPos = camera->GetSelectedCamera()->GetPosition();

	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();

	for (int i = 0; i < particles.size(); ++i)
	{
		EmitterInstance::Particle& particle = particles[i];

		if (particle.lifespan > 0.0f)
		{
			float3 translation = particle.tranform.TranslatePart();
			float3 zAxis = cameraPos - translation;
			zAxis.Normalize();
			float3 yAxis = float3::unitY;
			float3 xAxis = yAxis.Cross(zAxis);
			xAxis.Normalize();

			Quat rotation(zAxis, particle.rotation);

			xAxis = rotation.Transform(xAxis);
			yAxis = rotation.Transform(yAxis);

			xAxis *= particle.size;
			yAxis *= particle.size;
			zAxis *= particle.size;

			particle.tranform.SetCol3(0, xAxis);
			particle.tranform.SetCol3(1, yAxis);
			particle.tranform.SetCol3(2, zAxis);

			float3* matrix = reinterpret_cast<float3*>(instanceData + (i * stride));
			matrix[0] = xAxis;
			matrix[1] = yAxis;
			matrix[2] = zAxis;
			matrix[3] = translation;
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

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, instance->GetAliveParticles());

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

			const char* items[] = { "SCREEN", "WORLD", "AXIAL" };

			static const char* currentItem;
			switch (alignment)
			{
			case ModuleRenderer::Alignment::SCREEN:
				currentItem = items[0];
				break;
			case ModuleRenderer::Alignment::WORLD:
				currentItem = items[1];
				break;
			case ModuleRenderer::Alignment::AXIAL:
				currentItem = items[2];
				break;
			}

			if (ImGui::BeginCombo("##alignmentCombo", currentItem))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					// You can store your selection however you want, outside or inside your objects
					bool isSelected = (currentItem == items[n]);
					if (ImGui::Selectable(items[n], isSelected))
					{
						currentItem = items[n];
						if (isSelected)
						{
							// You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				if (currentItem == items[0])
				{
					alignment = ModuleRenderer::Alignment::SCREEN;
				}
				else if (currentItem == items[1])
				{
					alignment = ModuleRenderer::Alignment::WORLD;
				}
				else if (currentItem == items[2])
				{
					alignment = ModuleRenderer::Alignment::AXIAL;
				}

				SetAlignment(alignment);

				ImGui::EndCombo();
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
