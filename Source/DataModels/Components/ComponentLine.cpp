#include "StdAfx.h"
#include "ComponentLine.h"
#include "Application.h"
#include "Components/ComponentTransform.h"
#include "FileSystem/Json.h"
#include "GameObject/GameObject.h"
#include "ModuleCamera.h"
#include "Camera/Camera.h"
#include "ModuleProgram.h"
#include <GL/glew.h>

ComponentLine::ComponentLine(const bool active, GameObject* owner) : Component(ComponentType::LINE, active, owner, true)
{
	childGameObject = new GameObject("Line End", owner);
	childGameObject->InitNewEmptyGameObject(true);
	ComponentTransform* transform = childGameObject->GetComponent<ComponentTransform>();
	transform->SetPosition(
		float3(transform->GetPosition().x + 1.0, transform->GetPosition().y, transform->GetPosition().z));
	transform->UpdateTransformMatrices();
	LoadBuffers();
	UpdateBuffers();
}

ComponentLine::~ComponentLine()
{
}

//void ComponentLine::Update()
//{
//	RecalculateVertices();
//	FaceCamera();
//}

void ComponentLine::LoadBuffers()
{
	//Here will be the generation of all buffers, we can put them in the UpdateBuffers later but for now i split it
	float vertices[] = { 0.0f, 0.0f, 0.0f,
						 0.0f, 1.0f, 0.0f,
						 1.0f, 0.0f, 0.0f,
						 1.0f, 1.0f, 0.0f };

	glGenBuffers(1, &lineEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);

	glGenBuffers(1, &positionBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffers);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (numTiles * 2 + 2), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &textureBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffers);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &colorBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffers);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void ComponentLine::Draw() const
{
}

void ComponentLine::UpdateBuffers()
{
	//Here you update vbo,ebo and vao
	if (dirtyBuffers)
	{
		float step = 1.0f / float(numTiles);

		//vbo for position
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffers);
		float3* posData = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		float lambda = 1.0f;
		//float size = sizeOverTimeRange[0];
		posData[0] = float3(0.0f, 1 * -0.5f, 0.0f);
		posData[1] = float3(0.0f, 1 * 0.5f, 0.0f);

		for (unsigned int i = 0; i < numTiles; ++i)
		{
			lambda = step * float(i + 1);
			//size = sizeOverTimeRange[0] + (sizeOverTimeRange[1] - sizeOverTimeRange[0]) * ImGui::BezierValue(lambda, reinterpret_cast<float*>(&sizeOverTimePoints));
			posData[i * 2 + 2 + 0] = float3(lambda, 1.0 * -0.5f, 0.0f);
			posData[i * 2 + 2 + 1] = float3(lambda, 1.0 * 0.5f, 0.0f);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		//vbo for texture
		glBindBuffer(GL_ARRAY_BUFFER, textureBuffers);
		float2* texData = reinterpret_cast<float2*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		texData[0] = float2(0.0f, 0.0f);
		texData[1] = float2(0.0f, 1.0f);

		for (unsigned int i = 0; i < numTiles; ++i)
		{
			texData[i * 2 + 2 + 0] = float2(step * float(i + 1), 0.0f);
			texData[i * 2 + 2 + 1] = float2(step * float(i + 1), 1.0f);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		//vbo for color
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffers);
		float3* colorData = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		lambda = 0.0f;
		float3 color = { 1.0,0.0,0.0 };
		//colorGradient.gradient.getColorAt(lambda, (float*)&color);
		colorData[0] = color;
		colorData[1] = color;

		for (unsigned int i = 0; i < numTiles; ++i)
		{
			lambda = step * float(i + 1);
			//colorGradient.gradient.getColorAt(lambda, (float*)&color);
			colorData[i * 2 + 2 + 0] = color;
			colorData[i * 2 + 2 + 1] = color;
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		//ebo
		unsigned int* indexPtr = reinterpret_cast<unsigned*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
		indexPtr[0] = 0;
		indexPtr[1] = 1;

		for (unsigned int i = 0; i < numTiles; ++i)
		{
			indexPtr[i * 2 + 2 + 0] = i * 2 + 2;
			indexPtr[i * 2 + 2 + 1] = i * 2 + 3;
		}

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		dirtyBuffers = false;
	}

}

void ComponentLine::Render()
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::COMPONENT_LINE);
	if (program != nullptr && childGameObject != nullptr)
	{
		program->Activate();
		ModelMatrix(program);

		//I commented CW and CCW because they destroy the skybox need to look at that later
		// 
		//glFrontFace(GL_CW);
		//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glBindVertexArray(lineVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 + 2 * numTiles);
		glBindVertexArray(0);
		program->Deactivate();
		//glFrontFace(GL_CCW);
	}
}

void ComponentLine::ModelMatrix(Program* program)
{
	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	float3 globalPosition = GetOwner()->GetComponent<ComponentTransform>()->GetGlobalPosition();
	float3 childGlobalPosition = childGameObject->GetComponent<ComponentTransform>()->GetGlobalPosition();

	float3 middlePoint = (childGlobalPosition + globalPosition) / 2;
	float3 centerCamera = (App->GetModule<ModuleCamera>()->GetCamera()->GetPosition() - middlePoint).Normalized();
	float3 xAxis = childGlobalPosition - globalPosition;
	float3 normalized = (childGlobalPosition - globalPosition).Normalized();
	float3 yAxis = centerCamera.Cross(normalized);
	float3 zAxis = normalized.Cross(yAxis);

	const float4x4& model =
		float4x4(float4(xAxis, 0.0),
			float4(yAxis, 0.0),
			float4(zAxis, 0.0),
			GetOwner()->GetComponent<ComponentTransform>()->GetGlobalMatrix().Col(3));

	program->BindUniformFloat4x4(0, reinterpret_cast<const float*>(&proj), true);
	program->BindUniformFloat4x4(1, reinterpret_cast<const float*>(&view), true);
	program->BindUniformFloat4x4(2, reinterpret_cast<const float*>(&model), true);
}

void ComponentLine::RecalculateVertices()
{	
	const ComponentTransform* childTransform = childGameObject->GetComponent<ComponentTransform>();
	const ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();

	float stepsX = (childTransform->GetGlobalPosition().x - transform->GetGlobalPosition().x) / numTiles;
	float stepsY = (childTransform->GetGlobalPosition().y - transform->GetGlobalPosition().y) / numTiles;
	float stepsZ = (childTransform->GetGlobalPosition().z - transform->GetGlobalPosition().z) / numTiles;

	float vertices[] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, stepsX, stepsY, stepsZ, stepsX, stepsY + 1, stepsZ };

	std::vector<float> newVertices = std::vector<float>();
	newVertices.insert(newVertices.end(), &vertices[0], &vertices[sizeof(vertices) / sizeof(int)]);
	for (int i = 1; i < numTiles; i++)
	{
		float newPos = i + 1;
		newVertices.push_back(newPos * stepsX);
		newVertices.push_back(newPos * stepsY);
		newVertices.push_back(newPos * stepsZ);
		newVertices.push_back(newPos * stepsX);
		newVertices.push_back((newPos * stepsY) + 1);
		newVertices.push_back(newPos * stepsZ);
	}

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffers);
	glBufferData(GL_ARRAY_BUFFER, newVertices.size() * sizeof(float), &newVertices[0], GL_STATIC_DRAW);
}

void ComponentLine::FaceCamera()
{
	//Don't get this one
}