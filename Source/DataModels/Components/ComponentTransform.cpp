#include "ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "Application.h"
#include "Modules/ModuleScene.h"

#include "imgui.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner), ownerParent(GetOwner()->GetParent())
{
}

ComponentTransform::~ComponentTransform()
{
	delete ownerParent;
}


void ComponentTransform::Update()
{
	ownerParent = GetOwner()->GetParent();
	CalculateLocalMatrix();
	CalculateGlobalMatrix();
}

void ComponentTransform::Display()
{
	if (App->scene->GetRoot() == this->GetOwner()) // The root must not be moved through the inspector
		return;

	float3 translation = GetPosition();
	float3 rotation = RadToDeg(GetRotation().ToEulerXYZ());
	float3 scale = GetScale();

	ImGui::Text("TRANSFORM");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));

	if (ImGui::BeginTable("TransformTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Translation"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##XTrans", &translation.x, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YTrans", &translation.y, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZTrans", &translation.z, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar();

		ImGui::TableNextColumn();
		ImGui::Text("Rotation"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##XRot", &rotation.x, 0.05f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YRot", &rotation.y, 0.05f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZRot", &rotation.z, 0.05f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar();

		ImGui::TableNextColumn();
		ImGui::Text("Scale"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##XScale", &scale.x, 0.01f,
			0.0001f, std::numeric_limits<float>::max()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YScale", &scale.y, 0.01f,
			0.0001f, std::numeric_limits<float>::max()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZScale", &scale.z, 0.01f,
			0.0001f, std::numeric_limits<float>::max()
		); ImGui::PopStyleVar();

		SetPosition(translation);
		SetRotation(rotation);
		SetScale(scale);

		ImGui::EndTable();
		ImGui::Separator();
	}
}

void ComponentTransform::CalculateLocalMatrix()
{
	float4x4 localMatrix = float4x4::FromTRS((float3)GetPosition(), (Quat)GetRotation(), (float3)GetScale());

	SetLocalMatrix(localMatrix);
}

void ComponentTransform::CalculateGlobalMatrix()
{
	assert(ownerParent != nullptr);

	float3 parentPos, parentSca, localPos, localSca;
	Quat parentRot, localRot;

	ComponentTransform* parentTransform = (ComponentTransform*)ownerParent->GetComponent(ComponentType::TRANSFORM);

	parentTransform->GetGlobalMatrix().Decompose(parentPos, parentRot, parentSca);
	GetLocalMatrix().Decompose(localPos, localRot, localSca);

	float3 position = localPos + parentPos;
	Quat rotation = localRot * parentRot;
	float3 scale = parentSca.Mul(localSca);

	float4x4 globalMatrix = float4x4::FromTRS(position, rotation, scale);
	SetGlobalMatrix(globalMatrix);
}