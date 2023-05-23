#include "ModuleBase.h"

#include "EmitterInstance.h"
#include "ParticleEmitter.h"

#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "ImGui/imgui.h"

#include "debugdraw.h"

ModuleBase::ModuleBase(ParticleEmitter* emitter) : ParticleModule(ModuleType::BASE, emitter)
{
	originTransform = float4x4::identity;
	originLocation = DEFAULT_ORIGIN;
	originRotation = Quat::identity;
}

ModuleBase::~ModuleBase()
{
}

void ModuleBase::Spawn(EmitterInstance* instance)
{
}

void ModuleBase::Update(EmitterInstance* instance)
{
	const GameObject* go = instance->GetOwner()->GetOwner();
	ComponentTransform* objectTransform = static_cast<ComponentTransform*>(go->GetComponent(ComponentType::TRANSFORM));

	if (originTransform.IsIdentity())
	{
		originTransform = float4x4::FromTRS(originLocation, originRotation, float3::one);
	}

	float4x4 globalTransform = objectTransform->GetGlobalMatrix().Mul(originTransform);

	std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();

	for (int i = 0; i < particles.size(); ++i)
	{
		EmitterInstance::Particle& particle = particles[i];

		if (particle.tranform.IsIdentity() || particle.lifespan == 0.0f)
		{
			float radius = emitter->GetRadius();

			switch (emitter->GetShape())
			{
				case ParticleEmitter::ShapeType::CIRCLE:
				{
					float angle = math::RadToDeg(instance->CalculateRandomValueInRange(0.0f, 1.0f));
					float length = instance->CalculateRandomValueInRange(0.0f, 1.0f) * radius;

					float3 direction = float3(cos(angle), sin(angle), 0.0f);
					float3 point = direction * length;
					float4x4 pointTransform = float4x4::FromTRS(point, Quat::identity, float3::one);

					particle.tranform = globalTransform * pointTransform;
					particle.direction = (particle.tranform.TranslatePart() - 
						globalTransform.TranslatePart()).Normalized();

					break;
				}
				case ParticleEmitter::ShapeType::CONE:
				{
					float angle = math::RadToDeg(instance->CalculateRandomValueInRange(0.0f, 1.0f));
					float length = instance->CalculateRandomValueInRange(0.0f, 1.0f) * radius;

					float3 direction = float3(cos(angle), sin(angle), 0.0f);
					float3 point = direction * length;
					float4x4 pointTransform = float4x4::FromTRS(point, Quat::identity, float3::one);

					float baseRadius = math::Tan(math::DegToRad(emitter->GetAngle())) * CONE_HEIGHT + radius;
					float lengthAux = length * baseRadius / radius;

					float3 pointAux = direction * lengthAux;
					pointAux.z += CONE_HEIGHT;

					float4x4 pointAuxTransform = float4x4::FromTRS(pointAux, Quat::identity, float3::one);
					float4x4 globalAuxTransf = globalTransform * pointAuxTransform;

					particle.tranform = globalTransform * pointTransform;
					particle.direction = (globalAuxTransf.TranslatePart() - 
						particle.tranform.TranslatePart()).Normalized();

					break;
				}
				case ParticleEmitter::ShapeType::BOX:
				{
					float xPos = instance->CalculateRandomValueInRange(-radius/2, radius/2);
					float yPos = instance->CalculateRandomValueInRange(-radius/2, radius/2);
					float zPos = instance->CalculateRandomValueInRange(-radius/2, radius/2);

					float4x4 pointTransform = float4x4::FromTRS(float3(xPos, yPos, zPos), Quat::identity, float3::one);

					particle.tranform = globalTransform * pointTransform;
					particle.direction = (particle.tranform.TranslatePart() - 
						globalTransform.TranslatePart()).Normalized();

					break;
				}
			}

			float2 speed = emitter->GetSpeedRange();
			float velocity = emitter->IsRandomSpeed() ? 
				instance->CalculateRandomValueInRange(speed.x, speed.y) : speed.x;
			particle.initVelocity = particle.direction * velocity;
		}
	}
}

void ModuleBase::DrawDD(EmitterInstance* instance)
{
	const GameObject* go = instance->GetOwner()->GetOwner();
	ComponentTransform* objectTransform = static_cast<ComponentTransform*>(go->GetComponent(ComponentType::TRANSFORM));
	float4x4 globalTransform = objectTransform->GetGlobalMatrix().Mul(originTransform);

	float3 position;
	Quat rotation;
	float3 scale;
	globalTransform.Decompose(position, rotation, scale);

	float radius = emitter->GetRadius();
	float angle = emitter->GetAngle();

	float baseRadius = math::Tan(math::DegToRad(angle)) * CONE_HEIGHT + radius;

	switch (emitter->GetShape())
	{
	case ParticleEmitter::ShapeType::CIRCLE:
		dd::circle(position, globalTransform.WorldZ(), dd::colors::HotPink, radius, 25);
		break;
	case ParticleEmitter::ShapeType::CONE:
		dd::cone(position, globalTransform.WorldZ() * CONE_HEIGHT, dd::colors::HotPink, baseRadius, radius);
		break;
	case ParticleEmitter::ShapeType::BOX:
		dd::box(position, dd::colors::HotPink, radius, radius, radius);
		break;
	}
}

void ModuleBase::DrawImGui()
{
	if (ImGui::TreeNodeEx("Base Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		if (ImGui::BeginTable("##baseTable", 2))
		{
			bool updateTransform = false;

			float3 rotation = originRotation.ToEulerXYZ();
			rotation = RadToDeg(rotation);

			ImGui::TableNextColumn();
			ImGui::Text("Origin");

			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XTrans", &originLocation.x, 0.01f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
				updateTransform = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YTrans", &originLocation.y, 0.01f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
				updateTransform = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZTrans", &originLocation.z, 0.01f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
				updateTransform = true;
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			ImGui::Text("Rotation");

			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XRot", &rotation.x, 0.01f, -179.99f, 179.99f))
			{
				rotation = DegToRad(rotation);
				SetRotation(Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z));

				updateTransform = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YRot", &rotation.y, 0.01f, -179.99f, 179.99f))
			{
				rotation = DegToRad(rotation);
				SetRotation(Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z));

				updateTransform = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZRot", &rotation.z, 0.01f, -179.99f, 179.99f))
			{
				rotation = DegToRad(rotation);
				SetRotation(Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z));

				updateTransform = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			if (updateTransform)
			{
				originTransform = float4x4::FromTRS(originLocation, originRotation, float3::one);
			}

			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
