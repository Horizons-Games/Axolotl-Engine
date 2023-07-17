#include "StdAfx.h"
#include "Trail.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleProgram.h"

#include "Camera/Camera.h"
#include "Program/Program.h"

#include "Resources/ResourceTexture.h"

#include "FileSystem/Json.h"
#include "ImGui/imgui_color_gradient.h"


Trail::Trail() : maxSamplers(64), duration(2.5f), minDistance(1.f), width(10.f)
{
	points.reserve(maxSamplers);
	gradient = new ImGradient();
}

Trail::~Trail()
{
	points.clear();

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void Trail::Update(float3 newPosition, Quat newRotation)
{
	UpdateLife();

	if (CheckDistance(newPosition))
	{
		InsertPoint(newPosition, newRotation);
	}
	
}

void Trail::Draw()
{
	//if (isRendering)
	//{
		RedoBuffers();

		Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::DEFAULT); // TODO change to componentTrail or create one
		program->Activate();
		BindCamera(program);

		//time += App->GetDeltaTime() * speed;

		//will be move in AnimetedTexture Script or something similar
		//offset.x += 0.004f;
		//if (offset.x > 1.f) offset.x = 0.f;

		//program->BindUniformFloat2("offset", offset);
		//program->BindUniformFloat2("tiling", tiling);
		//program->BindUniformFloat("time", time);

		glActiveTexture(GL_TEXTURE0);
		if (texture)
		{
			texture->Load();
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			program->BindUniformInt("hasTexture", 1);
		}
		else
		{
			program->BindUniformInt("hasTexture", 0);
		}

		//I commented CW and CCW because they destroy the skybox need to look at that later
		// 
		//glFrontFace(GL_CW);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 2 * points.size());
		//glDrawElements(GL_TRIANGLE_STRIP, 2 + 2 * numTiles, GL_UNSIGNED_INT, nullptr);
		//glFrontFace(GL_CCW);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		program->Deactivate();
	//}
}

void Trail::CreateBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);

	unsigned vertexsSize = sizeof(Vertex);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glEnableVertexAttribArray(0); // pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexsSize, (void*)0);

	glEnableVertexAttribArray(1); // color
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexsSize, (void*)(sizeof(float3)));

	glEnableVertexAttribArray(2); // uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexsSize, (void*)((sizeof(float3)) + sizeof(float4)));

	glBindVertexArray(0);
}

void Trail::RedoBuffers()
{
	maxSamplers = std::max(maxSamplers, static_cast<int>(points.size()));
	unsigned maxTriangles = (maxSamplers - 1) * 2;
	GLuint maxIndices = maxTriangles * 3;
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndices, nullptr, GL_STATIC_DRAW);

	GLuint* indices = (GLuint*)(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (int i = 0; i < maxSamplers - 1; i++)
	{
		*(indices++) = 0 + 2 * i;
		*(indices++) = 1 + 2 * i;
		*(indices++) = 2 + 2 * i;
		*(indices++) = 1 + 2 * i;
		*(indices++) = 3 + 2 * i;
		*(indices++) = 2 + 2 * i;
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	int numVertices = maxSamplers * 2;

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, nullptr, GL_STATIC_DRAW);

	Vertex* vertexData = reinterpret_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	float stepUV = 1.0f / float(points.size());
	float stepsGradient = 1.f / static_cast<float>(points.size());
	float color[3];
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		Point p = points[i];
		
		// pos
		float3 dirPerpendicular = (p.rotation * float3::unitY) * width;
		float3 vertex = p.centerPosition + dirPerpendicular;
		vertexData[i * 2].position = vertex;
		vertex = p.centerPosition - dirPerpendicular;
		vertexData[i * 2 + 1].position = vertex;

		// uv
		vertexData[i * 2].uv = float2(stepUV * static_cast<float>(i), 0.0f);
		vertexData[i * 2 + 1].uv = float2(stepUV * static_cast<float>(i), 1.0f);

		// color
		gradient->getColorAt(stepsGradient * i, color);
		vertexData[i * 2].color = float4(color[0], color[1], color[2], p.life / duration);
		vertexData[i * 2 + 1].color = float4(color[0], color[1], color[2], p.life / duration);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Trail::UpdateLife()
{
	float timePassed = App->GetDeltaTime();
	for (int i = 0; i < points.size(); i++)
	{
		points[i].life -= timePassed;
	}

	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].life <= 0 && points[i + 1].life <= 0)
		{
			points.erase(points.begin() + i);
		}
	}
}

bool Trail::CheckDistance(float3 comparedPosition)
{
	return points.back().centerPosition.DistanceSq(comparedPosition) >= minDistance;
}

void Trail::InsertPoint(float3 position, Quat rotation)
{
	Point nPoint = Point();
	nPoint.centerPosition = position;
	nPoint.rotation = rotation;
	nPoint.life = duration;

	points.push_back(nPoint);
}

void Trail::BindCamera(Program* program)
{
	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	program->BindUniformFloat4x4(0, proj.ptr(), true);
	program->BindUniformFloat4x4(1, view.ptr(), true);
}
