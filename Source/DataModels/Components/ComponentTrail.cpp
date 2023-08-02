#include "StdAfx.h"
#include "ComponentTrail.h"
#include "ComponentTransform.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleProgram.h"

#include "Camera/Camera.h"
#include "Program/Program.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "ImGui/imgui_color_gradient.h"

#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceTexture.h"

#include "Enums/BlendingType.h"

#include "debugdraw.h"


ComponentTrail::ComponentTrail(bool active, GameObject* owner) : Component(ComponentType::TRAIL, active, owner, true),
maxSamplers(64), duration(10000.f), minDistance(0.1f), width(1.f), ratioWidth(0.5f), blendingMode(BlendingMode::ADDITIVE),
onPlay(false)
{ 
	points.reserve(maxSamplers);
	gradient = new ImGradient();

	CreateBuffers();
}

ComponentTrail::~ComponentTrail()
{
	points.clear();

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void ComponentTrail::Update()
{
	UpdateLife();

	ComponentTransform* trans = GetOwner()->GetComponent<ComponentTransform>();
	float3 newPosition = trans->GetGlobalPosition();
	Quat newRotation = trans->GetGlobalRotation();

	if (onPlay && CheckDistance(newPosition))
	{
		InsertPoint(newPosition, newRotation);
	}
}

void ComponentTrail::Render()
{
	if (points.size() < 2)
	{
		return;
	}

	RedoBuffers();

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::TRAIL);
	program->Activate();
	BindCamera(program);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

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

	if (texture)
	{
		texture->Load();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
		program->BindUniformInt(3, 1);
	}
	else
	{
		program->BindUniformInt(3, 0);
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glDrawElements(GL_TRIANGLES, (points.size() - 1) * 2 * 3, GL_UNSIGNED_INT, nullptr);
	program->Deactivate();

	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	glDisable(GL_BLEND);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentTrail::Draw() const
{
	bool canDrawLight = false 
#ifdef ENGINE
		|| IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject()
#endif
		;

	if (!canDrawLight)
	{
		return;
	}

	ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
	float3 position = transform->GetGlobalPosition();
	float3 dirPerpendicular = (transform->GetGlobalRotation() * float3::unitY) * width;
	float3 pos1 = position + dirPerpendicular;
	float3 pos2 = position - dirPerpendicular;

	dd::line(pos2, pos1, dd::colors::RebeccaPurple);
}

void ComponentTrail::InternalSave(Json& meta)
{
	meta["duration"] = static_cast<float>(duration);
	meta["minDistance"] = static_cast<float>(minDistance);
	meta["width"] = static_cast<float>(width);
	meta["ratioWidth"] = static_cast<float>(ratioWidth);
	meta["numberOfMarks"] = static_cast<int>(gradient->getMarks().size());
	
	std::list<ImGradientMark*> marks = gradient->getMarks();
	int i = 0;
	Json jsonColors = meta["ColorsGradient"];
	for (ImGradientMark* const& mark : marks)
	{
		jsonColors[i]["color_x"] = static_cast<float>(mark->color[0]);
		jsonColors[i]["color_y"] = static_cast<float>(mark->color[1]);
		jsonColors[i]["color_z"] = static_cast<float>(mark->color[2]);
		jsonColors[i]["color_w"] = static_cast<float>(mark->color[3]);
		jsonColors[i]["pos"] = static_cast<float>(mark->position);
		i++;
	}

	UID uid = 0;
	std::string assetPath = "";
	if (texture)
	{
		uid = texture->GetUID();
		assetPath = texture->GetAssetsPath();
	}

	meta["textureUID"] = static_cast<UID>(uid);
	meta["assetPathTexture"] = assetPath.c_str();
}

void ComponentTrail::InternalLoad(const Json& meta)
{
	duration = static_cast<float>(meta["duration"]);
	minDistance = static_cast<float>(meta["minDistance"]);
	width = static_cast<float>(meta["width"]);
	ratioWidth = static_cast<float>(meta["ratioWidth"]);
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
#ifdef ENGINE
	std::string path = meta["assetPathTexture"];
	bool materialExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());

	if (materialExists)
	{
		texture = App->GetModule<ModuleResources>()->RequestResource<ResourceTexture>(path);
	}
#else
	UID uidTexture = meta["textureUID"];
	std::shared_ptr<ResourceTexture> resourceTexture =
		App->GetModule<ModuleResources>()->SearchResource<ResourceTexture>(uidTexture);

	if (resourceTexture)
	{
		texture = resourceTexture;
	}
#endif // ENGINE
}

void ComponentTrail::CreateBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	unsigned maxTriangles = (maxSamplers - 1) * 2;
	GLuint maxIndices = maxTriangles * 3;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * maxIndices, nullptr, GL_STATIC_DRAW);

	unsigned vertexsSize = sizeof(Vertex);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	int numVertices = maxSamplers * 2;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, nullptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsSize, (void*)0);

	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexsSize, (void*)(sizeof(float3)));

	glEnableVertexAttribArray(2); // uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexsSize, (void*)((sizeof(float3)) + sizeof(float4)));

	glBindVertexArray(0);
}

void ComponentTrail::RedoBuffers()
{
	bool sizeChanged = false;

	if (maxSamplers < static_cast<int>(points.size()))
	{
		maxSamplers = static_cast<int>(points.size());
		sizeChanged = true;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	if (sizeChanged)
	{
		unsigned maxTriangles = (maxSamplers - 1) * 2;
		GLuint maxIndices = maxTriangles * 3;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * maxIndices, nullptr, GL_STATIC_DRAW);
	}

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	unsigned int index_idx = 0;
	for (int i = 0; i < maxSamplers - 1; i++)
	{
		indices[index_idx++] = 0 + 2 * i;
		indices[index_idx++] = 2 + 2 * i;
		indices[index_idx++] = 3 + 2 * i;
		indices[index_idx++] = 3 + 2 * i;
		indices[index_idx++] = 1 + 2 * i;
		indices[index_idx++] = 0 + 2 * i;
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if (sizeChanged)
	{
		int numVertices = maxSamplers * 2;
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, nullptr, GL_STATIC_DRAW);
	}

	Vertex* vertexData = reinterpret_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	float steps = 1.0f / float(points.size());
	float3 color;
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		Point p = points[i];

		// pos
		float ratioLife = p.life / duration;
		float lerpWidht = Lerp(ratioWidth * width, width, ratioLife);
		float3 dirPerpendicular = (p.rotation * float3::unitY) * lerpWidht;
		float3 vertex = p.centerPosition + dirPerpendicular;
		vertexData[i * 2].position = vertex;
		vertex = p.centerPosition - dirPerpendicular;
		vertexData[i * 2 + 1].position = vertex;

		// uv
		vertexData[i * 2].uv = float2(steps * static_cast<float>(i), 1.0f);
		vertexData[i * 2 + 1].uv = float2(steps * static_cast<float>(i), 0.0f);

		// color
		gradient->getColorAt(steps * i, color.ptr());
		vertexData[i * 2].color = float4(color, p.life / duration);
		vertexData[i * 2 + 1].color = float4(color, p.life / duration);
		//if (blendingMode == BlendingMode::ADDITIVE)
		//{
		//	// Additive alpha lerp to black
		//	color = color.Lerp(float3(0.0f, 0.0f, 0.0f), 1.0f - vertexData[i].color.w);
		//}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ComponentTrail::UpdateLife()
{
	float timePassed = App->GetDeltaTime();
	for (int i = 0; i < points.size(); i++)
	{
		points[i].life -= timePassed;
	}

	for (int i = 0; i < points.size(); i++)
	{
		if (i == points.size() - 1 && points[i].life <= 0)
		{
			points.erase(points.begin() + i);
		}
		else if (points[i].life <= 0 && points[i + 1].life <= 0)
		{
			points.erase(points.begin() + i);
		}
	}
}

bool ComponentTrail::CheckDistance(float3 comparedPosition)
{
	return points.empty() || points.back().centerPosition.DistanceSq(comparedPosition) >= minDistance;
}

void ComponentTrail::InsertPoint(float3 position, Quat rotation)
{
	Point nPoint = Point();
	nPoint.centerPosition = position;
	nPoint.rotation = rotation;
	nPoint.life = duration;

	points.push_back(nPoint);
}

void ComponentTrail::BindCamera(Program* program)
{
	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	program->BindUniformFloat4x4(0, proj.ptr(), true);
	program->BindUniformFloat4x4(1, view.ptr(), true);
}
