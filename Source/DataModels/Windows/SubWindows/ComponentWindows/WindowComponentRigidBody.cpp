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
	}
}
