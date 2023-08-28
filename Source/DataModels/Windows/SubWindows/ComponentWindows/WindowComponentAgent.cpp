#include "StdAfx.h"

#include "WindowComponentAgent.h"

#include "Components/ComponentAgent.h"

WindowComponentAgent::WindowComponentAgent(ComponentAgent* component) : ComponentWindow("AGENT", component)
{
}

WindowComponentAgent::~WindowComponentAgent()
{
}

void WindowComponentAgent::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentAgent* asAgent = static_cast<ComponentAgent*>(component);

	if (asAgent)
	{
		//float maxSpeed = asAgent->GetMaxSpeed();
		float maxAcceleration = asAgent->GetMaxAcceleration();
		float yOffset = asAgent->GetYOffset();
		//float rotationSpeed = asAgent->GetRotationSpeed();
		bool avoidingObstacle = asAgent->IsAvoidingObstacle();
		bool updateRigidBody = asAgent->GetUpdateRigidBody();

		/*if (ImGui::DragFloat("Agent max speed", &maxSpeed, 0.1f, 0.0f, FLT_MAX))
		{
			asAgent->SetMaxSpeed(maxSpeed);
		}*/

		if (ImGui::DragFloat("Agent max acceleration", &maxAcceleration, 0.1f, 0.0f, FLT_MAX))
		{
			asAgent->SetMaxAcceleration(maxAcceleration);
		}
		
		/*if (ImGui::DragFloat("Agent rotation speed", &rotationSpeed, 0.1f, 0.0f, 5.0f))
		{
			asAgent->SetRotationSpeed(rotationSpeed);
		}*/
		
		if (ImGui::DragFloat("Position Y offset", &yOffset, 0.1f, 0.0f, FLT_MAX))
		{
			asAgent->SetYOffset(yOffset);
		}

		if (ImGui::Checkbox("Obstacle Avoidance", &avoidingObstacle))
		{
			asAgent->SetAgentObstacleAvoidance(avoidingObstacle);
		}
		
		if (ImGui::Checkbox("Update owners RigidBody", &updateRigidBody))
		{
			asAgent->SetUpdateRigidBody(updateRigidBody);
		}
	}
}
