#include "WindowComponentRigidBody.h"

#include "Components/ComponentRigidBody.h"

WindowComponentRigidBody::WindowComponentRigidBody(ComponentRigidBody* component) :
	ComponentWindow("RIGID BODY", component)
{
}

WindowComponentRigidBody::~WindowComponentRigidBody()
{
}

void WindowComponentRigidBody::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentRigidBody* asRigidBody = static_cast<ComponentRigidBody*>(component);

	if (asRigidBody)
	{
		ImGui::Text("");

		bool isKinematic = asRigidBody->GetIsKinematic();

		ImGui::Text("Is Kinematic");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Is Kinematic", &isKinematic))
		{
			asRigidBody->SetIsKinematic(isKinematic);
		}

		float mass = asRigidBody->GetMass();

		ImGui::Text("Mass:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		if (ImGui::DragFloat("##Mass", &mass, 0.1f, 1.0f, 100.0f, "%.2f"))
		{
			if (mass < 1.0f)
			{
				mass = 1.0f;
			}

			asRigidBody->SetMass(mass);
		}

		float3 g = -asRigidBody->GetGravity();

		ImGui::Text("Gravity:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		if (ImGui::DragFloat("##Gravity", &g.y, 0.1f, 1.0f, 10.0f, "%.2f"))
		{
			if (g.y < 1.0f)
			{
				g.y = 1.0f;
			}

			g.y = -g.y;
			asRigidBody->SetGravity(g);
		}

		float KpForce = asRigidBody->GetKpForce();
		float KpTorque = asRigidBody->GetKpTorque();
		bool isUsingPositionController = asRigidBody->GetUsePositionController();
		bool isUsingRotationController = asRigidBody->GetUseRotationController();

		ImGui::Text("Use Position Controller");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Use Position Controller", &isUsingPositionController))
		{
			asRigidBody->SetUsePositionController(isUsingPositionController);
		}

		if (isUsingPositionController)
		{
			if (ImGui::SliderFloat("KP Position", &KpForce, 0.5f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			{
				asRigidBody->SetKpForce(KpForce);
			}
		}

		ImGui::Text("Use Rotation Controller");
		ImGui::SameLine();
		if (ImGui::Checkbox("##Use Rotation Controller", &isUsingRotationController))
		{
			asRigidBody->SetUseRotationController(isUsingRotationController);
		}

		if (isUsingRotationController)
		{
			if (ImGui::SliderFloat("KP Rotation", &KpTorque, 0.05f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			{
				asRigidBody->SetKpTorque(KpTorque);
			}
		}
	}
}
