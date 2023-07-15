#include "StdAfx.h"
#include "ComponentLine.h"
#include "Application.h"
#include "Components/ComponentTransform.h"
#include "FileSystem/Json.h"
#include "GameObject/GameObject.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "Camera/Camera.h"
#include "ModuleProgram.h"
#include "Program/Program.h"
#include "Resources/ResourceTexture.h"
#include <GL/glew.h>

ComponentLine::ComponentLine(const bool active, GameObject* owner) : Component(ComponentType::LINE, active, owner, true),
	numTiles(1),speed(0.0f),time(0.0f),dirtyBuffers(true),offset(float2::zero),tiling(float2::one), gradient(new ImGradient()),
	sizeFading(float2::one),sizeFadingPoints(float4::zero)
{
	LoadBuffers();
	UpdateBuffers();
}

ComponentLine::~ComponentLine()
{
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1,&lineEBO);
	glDeleteBuffers(1,&positionBuffers);
	glDeleteBuffers(1,&textureBuffers);
	glDeleteBuffers(1,&colorBuffers);
}

void ComponentLine::LoadBuffers()
{
	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);

	glGenBuffers(1, &lineEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &positionBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffers);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);
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

void ComponentLine::UpdateBuffers()
{
	if (dirtyBuffers)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, positionBuffers);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textureBuffers);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, colorBuffers);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

		float step = 1.0f / float(numTiles);

		// VBO for position
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffers);
		float3* posData = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		float lambda = 0.0f;
		float size = sizeFading[0];
		posData[0] = float3(0.0f, size * -0.5f, 0.0f);
		posData[1] = float3(0.0f, size * 0.5f, 0.0f);

		for (unsigned int i = 0; i < numTiles; ++i)
		{
			lambda = step * float(i + 1);
			size = sizeFading[0] + (sizeFading[1] - sizeFading[0]) * 
				ImGui::BezierValue(lambda, reinterpret_cast<float*>(&sizeFadingPoints));
			posData[i * 2 + 2 + 0] = float3(lambda, size * -0.5f, 0.0f);
			posData[i * 2 + 2 + 1] = float3(lambda, size * 0.5f, 0.0f);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		// VBO for texture
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

		// VBO for color
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffers);
		float3* colorData = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		lambda = 0.0f;
		float stepsGradient = static_cast<float>(1) / static_cast<float>(numTiles);
		float color[3];
		gradient->getColorAt(0.0, color);
		colorData[0] = float3(color[0], color[1], color[2]);
		colorData[1] = float3(color[0], color[1], color[2]);
		for (unsigned int i = 0; i < numTiles; ++i)
		{
			gradient->getColorAt(stepsGradient * (i + 1), color);
			lambda = step * float(i + 1);
			colorData[i * 2 + 2 + 0] = float3(color[0], color[1], color[2]);
			colorData[i * 2 + 2 + 1] = float3(color[0], color[1], color[2]);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);

		// EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineEBO);
		unsigned int* indexPtr = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
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
	if (childGameObject != nullptr)
	{
		program->Activate();
		ModelMatrix(program);
		UpdateBuffers();

		program->BindUniformFloat2("offset", offset);
		program->BindUniformFloat2("tiling", tiling);
		program->BindUniformFloat("time", time);

		glActiveTexture(GL_TEXTURE0);
		if (lineTexture)
		{
			lineTexture->Load();
			glBindTexture(GL_TEXTURE_2D, lineTexture->GetGlTexture());
			program->BindUniformInt("hasTexture", 1);
		}
		else
		{
			program->BindUniformInt("hasTexture", 0);
		}

		glBindVertexArray(lineVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 + 2 * numTiles);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		program->Deactivate();
	}
}

void ComponentLine::ModelMatrix(Program* program)
{
	if (childGameObject)
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
}

void ComponentLine::InternalSave(Json& meta)
{
	meta["numTiles"] = static_cast<int>(numTiles);
	meta["speed"] = static_cast<float>(speed);
	meta["offset_x"] =  static_cast<float>(offset.x);
	meta["offset_y"] =  static_cast<float>(offset.y);
	meta["tiling_x"] =  static_cast<float>(tiling.x);
	meta["tiling_y"] =  static_cast<float>(tiling.y);
	meta["sizeFading_x"] = static_cast<float>(sizeFading.x);
	meta["sizeFading_y"] = static_cast<float>(sizeFading.y);
	meta["sizeFadingPoints_x"] = static_cast<float>(sizeFadingPoints.x);
	meta["sizeFadingPoints_y"] = static_cast<float>(sizeFadingPoints.y);
	meta["sizeFadingPoints_z"] = static_cast<float>(sizeFadingPoints.z);
	meta["sizeFadingPoints_w"] = static_cast<float>(sizeFadingPoints.w);
	meta["numberOfMarks"] = static_cast<int>( gradient->getMarks().size());
	std::list<ImGradientMark*> marks = gradient->getMarks();
	int i = 0;
	char charI;
	std::string value;
	Json jsonColors = meta["ColorsGradient"];
	for (ImGradientMark* const& mark : marks)
	{
		jsonColors[i]["color_x"] = static_cast<float>(mark->color[0]);
		jsonColors[i]["color_y"] = static_cast<float>(mark->color[1]);
		jsonColors[i]["color_z"] = static_cast<float>(mark->color[2]);
		jsonColors[i]["color_w"] = static_cast<float>(mark->color[3]);
		jsonColors[i]["pos"] =	   static_cast<float>(mark->position);
		i++;
	}
	UID uid = 0;
	std::string assetPath = "";
	if (lineTexture)
	{
		uid = lineTexture->GetUID();
		assetPath = lineTexture->GetAssetsPath();
	}

	meta["materialUID"] = static_cast<UID>(uid);
	meta["assetPathMaterial"] = assetPath.c_str();

	if (childGameObject)
	{
		uid = childGameObject->GetUID();
	}

	meta["EndPoint"] = static_cast<UID>(uid);
}

void ComponentLine::InternalLoad(const Json& meta)
{
	numTiles = static_cast<int>(meta["numTiles"]);
	speed = static_cast<float>(meta["speed"]);
	offset.x = static_cast<float>(meta["offset_x"]);
	offset.y = static_cast<float>(meta["offset_y"]);
	tiling.x = static_cast<float>(meta["tiling_x"]);
	tiling.y = static_cast<float>(meta["tiling_y"]);
	sizeFading.y = static_cast<float>(meta["sizeFading_y"]);
	sizeFading.x = static_cast<float>(meta["sizeFading_x"]);
	sizeFadingPoints.x = static_cast<float>(meta["sizeFadingPoints_x"]);
	sizeFadingPoints.y = static_cast<float>(meta["sizeFadingPoints_y"]);
	sizeFadingPoints.z = static_cast<float>(meta["sizeFadingPoints_z"]);
	sizeFadingPoints.w = static_cast<float>(meta["sizeFadingPoints_w"]);
	int numberOfMarks = static_cast<int>(meta["numberOfMarks"]);
	gradient->getMarks().clear();

	Json jsonColors = meta["ColorsGradient"];

	for (int i = 0; i < numberOfMarks; i++)
	{
		
		gradient->addMark(static_cast<float>(jsonColors[i]["pos"]), 
							ImColor(static_cast<float>(jsonColors[i]["color_x"]),
							static_cast<float>(jsonColors[i]["color_y"]),
							static_cast<float>(jsonColors[i]["color_z"]),
							static_cast<float>(jsonColors[i]["color_a"])));
	}
	gradient->refreshCache();
	std::string path = meta["assetPathMaterial"];
	bool materialExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());

	if (materialExists)
	{
		std::shared_ptr<ResourceTexture> lineTexture =
			App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(path);

		if (lineTexture)
		{
			SetLineTexture(lineTexture);
		}
	}

	UID endpoint = meta["EndPoint"];

	if (endpoint != 0)
	{
		childGameObject = App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(endpoint);
	}

	dirtyBuffers = true;
}

const GameObject* ComponentLine::GetEnd() const
{
	return childGameObject;
}

void ComponentLine::SetEnd(GameObject* end)
{
	childGameObject = end;
}
