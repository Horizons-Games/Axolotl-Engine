#include "ComponentLine.h"
#include "Application.h"
#include "Components/ComponentTransform.h"
#include "DataModels/Program/Program.h"
#include "FileSystem/Json.h"
#include "GameObject/GameObject.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include <GL/glew.h>

ComponentLine::ComponentLine(const bool active, GameObject* owner) : Component(ComponentType::LINE, active, owner, true)
{
	childGameObject = new GameObject("Line End", owner);
	childGameObject->InitNewEmptyGameObject(true);
	ComponentTransform* transform =
		static_cast<ComponentTransform*>(childGameObject->GetComponent(ComponentType::TRANSFORM));
	transform->SetPosition(
		float3(transform->GetPosition().x + 1.0, transform->GetPosition().y, transform->GetPosition().z));
	transform->UpdateTransformMatrices();
	LoadVBO();
}

ComponentLine::~ComponentLine()
{
}

void ComponentLine::Update()
{
	RecalculateVertices();
	FaceCamera();
}

void ComponentLine::LoadVBO()
{
	float vertices[] = { 0.0f, 0.0f, 0.0f,
						 0.0f, 1.0f, 0.0f, 
						 1.0f, 0.0f, 0.0f, 
						 1.0f, 1.0f, 0.0f	
	};
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void ComponentLine::Draw() const
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::HIGHLIGHT);
	if (program)
	{
		program->Activate();
		const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
		const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();
		const float4x4& model =
			static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

		glUniformMatrix4fv(2, 1, GL_TRUE, (const float*) &view);
		glUniformMatrix4fv(1, 1, GL_TRUE, (const float*) &model);
		glUniformMatrix4fv(0, 1, GL_TRUE, (const float*) &proj);
		glFrontFace(GL_CW);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 + 2 * numTiles);
		program->Deactivate();
		glFrontFace(GL_CCW);
	}
}

void ComponentLine::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;
}

void ComponentLine::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
}

void ComponentLine::RecalculateVertices()
{
	const ComponentTransform* childTransform =
		static_cast<ComponentTransform*>(childGameObject->GetComponent(ComponentType::TRANSFORM));

	const ComponentTransform* transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float stepsX = (childTransform->GetGlobalPosition().x - transform->GetGlobalPosition().x) / numTiles;
	float stepsY = (childTransform->GetGlobalPosition().y - transform->GetGlobalPosition().y) / numTiles;
	float stepsZ = (childTransform->GetGlobalPosition().z - transform->GetGlobalPosition().z) / numTiles;

	float vertices[] = { 0.0f, 0.0f, 0.0f,
						 0.0f, 1.0f, 0.0f, 
						 stepsX, stepsY, stepsZ, 
						 stepsX, stepsY + 1, stepsZ 
						};

	std::vector<float> newVertices = std::vector<float>();
	newVertices.insert(newVertices.end(), &vertices[0], &vertices[sizeof(vertices) / sizeof(int)]);
	for (int i = 1 ; i < numTiles ; i++)
	{
		float newPos = i + 1;
		newVertices.push_back(newPos * stepsX);
		newVertices.push_back(newPos * stepsY);
		newVertices.push_back(newPos * stepsZ);
		newVertices.push_back(newPos * stepsX);
		newVertices.push_back((newPos * stepsY) + 1);
		newVertices.push_back(newPos * stepsZ);

	}

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER,  newVertices.size() * sizeof(float), &newVertices[0], GL_STATIC_DRAW);

}

void ComponentLine::FaceCamera()
{

}