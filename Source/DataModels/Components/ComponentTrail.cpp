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

#define ALPHA_CENTR 0.5f

ComponentTrail::ComponentTrail(bool active, GameObject* owner) : Component(ComponentType::TRAIL, active, owner, true),
maxSamplers(64), duration(5.f), minDistance(0.1f), width(1.f), ratioWidth(0.5f), blendingMode(BlendingMode::ADDITIVE),
onPlay(true), catmullPoints(10)
{ 
	points.reserve(maxSamplers);
	gradient = new ImGradient();
	App->ScheduleTask([this]()
		{
			CreateBuffers();
		});
	
}

ComponentTrail::ComponentTrail(const ComponentTrail& trail) : Component(trail), maxSamplers(trail.maxSamplers), 
duration(trail.duration), minDistance(trail.minDistance), width(trail.width), ratioWidth(trail.ratioWidth), 
blendingMode(trail.blendingMode), onPlay(trail.onPlay), catmullPoints(trail.catmullPoints), texture(trail.texture)
{
	points.reserve(maxSamplers);
	gradient = new ImGradient(trail.gradient);
	CreateBuffers();
}

ComponentTrail::~ComponentTrail()
{
	points.clear();
	delete gradient;

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
	int totalCatmullPoints = catmullPoints * (static_cast<int>(points.size()) - 1);

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
	int size = (static_cast<int>(points.size()) - 1 + totalCatmullPoints) * 2 * 3;
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
	program->Deactivate();

	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	glDisable(GL_BLEND);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentTrail::Draw() const
{
	bool canDrawTrail = false 
#ifdef ENGINE
		|| IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject()
#endif
		;

	if (!canDrawTrail)
	{
		return;
	}

	ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
	float3 position = transform->GetGlobalPosition();
	float3 dirPerpendicular = (transform->GetGlobalRotation() * float3::unitY) * width;
	float3 pos1 = position + dirPerpendicular;
	float3 pos2 = position - dirPerpendicular;

	dd::line(pos2, pos1, dd::colors::Lime);
}

void ComponentTrail::InternalSave(Json& meta)
{
	meta["duration"] = duration;
	meta["minDistance"] = minDistance;
	meta["width"] = width;
	meta["catmullPoints"] = catmullPoints;
	meta["ratioWidth"] = ratioWidth;
	meta["onPlay"] = onPlay;
	
	std::list<ImGradientMark*> marks = gradient->getMarks();
	Json jsonColors = meta["ColorsGradient"];
	
	ImGradientMark* mark = marks.front();
	jsonColors[0]["color_x"] = mark->color[0];
	jsonColors[0]["color_y"] = mark->color[1];
	jsonColors[0]["color_z"] = mark->color[2];
	jsonColors[0]["color_w"] = mark->color[3];
	jsonColors[0]["pos"] = mark->position;

	mark = marks.back();
	jsonColors[1]["color_x"] = mark->color[0];
	jsonColors[1]["color_y"] = mark->color[1];
	jsonColors[1]["color_z"] = mark->color[2];
	jsonColors[1]["color_w"] = mark->color[3];
	jsonColors[1]["pos"] = mark->position;

	UID uid = 0;
	std::string assetPath = "";
	if (texture)
	{
		uid = texture->GetUID();
		assetPath = texture->GetAssetsPath();
	}

	meta["textureUID"] = uid;
	meta["assetPathTexture"] = assetPath.c_str();
}

void ComponentTrail::InternalLoad(const Json& meta)
{
	duration = static_cast<float>(meta["duration"]);
	minDistance = static_cast<float>(meta["minDistance"]);
	width = static_cast<float>(meta["width"]);
	ratioWidth = static_cast<float>(meta["ratioWidth"]);
	catmullPoints = static_cast<int>(meta["catmullPoints"]);
	onPlay = static_cast<bool>(meta["onPlay"]);
	
	gradient->getMarks().clear();
	Json jsonColors = meta["ColorsGradient"];
	gradient->addMark(static_cast<float>(jsonColors[0]["pos"]),
		ImColor(static_cast<float>(jsonColors[0]["color_x"]),
			  static_cast<float>(jsonColors[0]["color_y"]),
			  static_cast<float>(jsonColors[0]["color_z"]),
			  static_cast<float>(jsonColors[0]["color_w"])));

	gradient->addMark(static_cast<float>(jsonColors[1]["pos"]),
		ImColor(static_cast<float>(jsonColors[1]["color_x"]),
			static_cast<float>(jsonColors[1]["color_y"]),
			static_cast<float>(jsonColors[1]["color_z"]),
			static_cast<float>(jsonColors[1]["color_w"])));

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
	int totalCatmullPoints = catmullPoints * (maxSamplers - 1);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	unsigned maxTriangles = (maxSamplers - 1 + totalCatmullPoints) * 2;
	GLuint maxIndices = maxTriangles * 3;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * maxIndices, nullptr, GL_STATIC_DRAW);

	unsigned vertexsSize = sizeof(Vertex);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	int numVertices = (maxSamplers + totalCatmullPoints) * 2;
	glBufferData(GL_ARRAY_BUFFER, vertexsSize * numVertices, nullptr, GL_STATIC_DRAW);
	
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
	int totalCatmullPoints = catmullPoints * (static_cast<int>(points.size()) - 1);

	if (maxSamplers < static_cast<int>(points.size()))
	{
		maxSamplers = static_cast<int>(points.size());
		sizeChanged = true;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	if (sizeChanged)
	{
		unsigned maxTriangles = (maxSamplers - 1 + totalCatmullPoints) * 2;
		GLuint maxIndices = maxTriangles * 3;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * maxIndices, nullptr, GL_STATIC_DRAW);
	}

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	unsigned int index_idx = 0;
	for (int i = 0; i < (points.size() - 1) + totalCatmullPoints; i++)
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
		int numVertices = (maxSamplers + totalCatmullPoints) * 2;
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, nullptr, GL_STATIC_DRAW);
	}

	Vertex* vertexData = reinterpret_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	float steps = 1.0f / static_cast<float>(points.size() - 1 + totalCatmullPoints);
	float stepsCatmull = 1.0f / static_cast<float>(catmullPoints + 1);
	float3 color;
	std::list<ImGradientMark*> marks = gradient->getMarks();
	float3 initColor = float3(marks.front()->color);
	float3 endColor = float3(marks.back()->color);
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		int posInMemory = 2 * i * (1 + catmullPoints);
		float actualStep = static_cast<float>(i) * (1 + catmullPoints);

		Point p = points[i];

		// pos
		float ratioLife = p.life / duration;
		float lerpWidht = Lerp(ratioWidth * width, width, ratioLife);
		float3 dirPerpendicular = (p.rotation * float3::unitY) * lerpWidht;
		float3 vertex = p.centerPosition + dirPerpendicular;
		vertexData[posInMemory].position = vertex;
		vertex = p.centerPosition - dirPerpendicular;
		vertexData[posInMemory + 1].position = vertex;

		// uv
		vertexData[posInMemory].uv = float2(steps * static_cast<float>(actualStep), 1.0f);
		vertexData[posInMemory + 1].uv = float2(steps * static_cast<float>(actualStep), 0.0f);

		// color
		color = Lerp(initColor, endColor, steps * static_cast<float>(actualStep));
		if (blendingMode == BlendingMode::ADDITIVE)
		{
			// additive alpha lerp to black
			color = color.Lerp(float3(0.0f, 0.0f, 0.0f), 1.0f - ratioLife);
		}
		vertexData[posInMemory].color = float4(color, ratioLife);
		vertexData[posInMemory + 1].color = float4(color, ratioLife);

		if (i != points.size() - 1)
		{
			float3 p0, p3;
			Point p2 = points[i + 1];
			CalculateExtraPoints(p0, p, p2, p3);
			const Curve curve = CatmullRomCentripetal(p0, p.centerPosition, p2.centerPosition, p3);
			for (int j = 1; j <= catmullPoints; j++)
			{
				float lambda = stepsCatmull * j;
				float3 pointCatmull = curve.a * lambda * lambda * lambda + curve.b * lambda * lambda + curve.c * lambda
					+ curve.d;
				Quat rotationPoint = p.rotation.Lerp(p2.rotation, lambda);

				// pos
				lerpWidht = Lerp(ratioWidth * width, width, ratioLife);
				dirPerpendicular = (rotationPoint * float3::unitY) * lerpWidht;
				vertex = pointCatmull + dirPerpendicular;
				vertexData[posInMemory + j * 2].position = vertex;
				vertex = pointCatmull - dirPerpendicular;
				vertexData[posInMemory + j * 2 + 1].position = vertex;

				actualStep++;

				// uv
				vertexData[posInMemory + j * 2].uv = float2(steps * static_cast<float>(actualStep), 1.0f);
				vertexData[posInMemory + j * 2 + 1].uv = float2(steps * static_cast<float>(actualStep), 0.0f);

				// color
				color = Lerp(initColor, endColor, steps * static_cast<float>(actualStep));
				if (blendingMode == BlendingMode::ADDITIVE)
				{
					// additive alpha lerp to black
					color = color.Lerp(float3(0.0f, 0.0f, 0.0f), 1.0f - ratioLife);
				}
				vertexData[posInMemory + j * 2].color = float4(color, ratioLife);
				vertexData[posInMemory + j * 2 + 1].color = float4(color, ratioLife);
			}
		}
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

		if (points[i].life < 0.0f)
		{
			points[i].life = 0.0f;
		}
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

void ComponentTrail::CalculateExtraPoints(float3& p0, const Point& p1, const Point& p2, float3& p3)
{
	// Calculate the reflected point p0 with the point p2 and p1 as a surface
	float3 dir = p1.centerPosition - p2.centerPosition;
	float3 normal = (p1.rotation * float3::unitY).Normalized();
	p0 = dir - 2 * (dir.Dot(normal)) * normal;
	// Calculate now p3
	dir = p2.centerPosition - p1.centerPosition;
	normal = (p2.rotation * float3::unitY).Normalized();
	p3 = dir - 2 * (dir.Dot(normal)) * normal;
}

const Curve ComponentTrail::CatmullRomCentripetal(float3 p0, float3 p1, float3 p2, float3 p3)
{
	float t0 = 0.0f;
	float t1 = t0 + pow(p0.Distance(p1), ALPHA_CENTR);
	float t2 = t1 + pow(p1.Distance(p2), ALPHA_CENTR);
	float t3 = t2 + pow(p2.Distance(p3), ALPHA_CENTR);
	
	float3 m1 = (t2 - t1) *	((p1 - p0) / (t1 - t0) - (p2 - p0) / (t2 - t0) + (p2 - p1) / (t2 - t1));
	float3 m2 = (t2 - t1) * ((p2 - p1) / (t2 - t1) - (p3 - p1) / (t3 - t1) + (p3 - p2) / (t3 - t2));
	
	Curve curve;
	curve.a = 2.0f * (p1 - p2) + m1 + m2;
	curve.b = -3.0f * (p1 - p2) - m1 - m1 - m2;
	curve.c = m1;
	curve.d = p1;

	return curve;
}

void ComponentTrail::BindCamera(Program* program)
{
	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	program->BindUniformFloat4x4(0, proj.ptr(), true);
	program->BindUniformFloat4x4(1, view.ptr(), true);
}
