#include "StdAfx.h"
#include "Trail.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleProgram.h"

#include "Camera/Camera.h"
#include "Program/Program.h"

#include "Resources/ResourceTexture.h"

#include "FileSystem/Json.h"

Trail::Trail() : maxSamplers(10), redoBuffers(true)
{
	points.reserve(maxSamplers);
}

Trail::~Trail()
{
	points.clear();

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &uv);
}

void Trail::Update(float3 newPosition, Quat newRotation)
{
	if (!isRendering) // if we are not rendering the tail don't store
	{
		return;
	}

	if (CheckDistance(newPosition))
	{
		InsertPoint(newPosition, newRotation);
	}
	
	UpdateLife();
}

void Trail::Draw()
{
	if (isRendering)
	{
		if (redoBuffers)
		{
			RedoBuffers();
		}

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
	}
}

void Trail::CreateBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float2), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Trail::RedoBuffers()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * (maxSamplers * 2), nullptr, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (maxSamplers * 2), nullptr, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * (maxSamplers * 2), nullptr, GL_STATIC_DRAW);

	CalculateUV();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	redoBuffers = false;
}

void Trail::UpdateLife()
{
	for (auto &point : points)
	{
		point->life -= App->GetDeltaTime();
		if (point->life <= 0)
		{
			std::erase(points, point);
			CalculateVBO();
		}
	}
}

bool Trail::CheckDistance(float3 comparedPosition)
{
	return lastPosition.DistanceSq(comparedPosition) >= minDistance;
}

void Trail::InsertPoint(float3 position, Quat rotation)
{
	float3 dirPerpendicular = (rotation * float3::unitY) * widht;
	Point* nPoint = new Point();
	nPoint->centerPosition = position;
	nPoint->vertex[0] = position + dirPerpendicular;
	nPoint->vertex[1] = position - dirPerpendicular;
	nPoint->life = duration;

	if (points.size() + 1 >= maxSamplers) //if we have more than the number specified, delete the first one
	{
		points.erase(points.begin());
	}

	points.push_back(std::make_unique<Point>(*nPoint));

	CalculateVBO();
}

void Trail::CalculateUV()
{
	float step = 1.0f / float(points.size());

	glBindBuffer(GL_ARRAY_BUFFER, uv);
	float2* texData = reinterpret_cast<float2*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		texData[i * 2 + 0] = float2(step * float(i), 0.0f);
		texData[i * 2 + 1] = float2(step * float(i), 1.0f);
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Trail::CalculateVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	float3* posData = reinterpret_cast<float3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		Point* p = points[i].get();
		posData[i * 2 + 0] = p->vertex[0];
		posData[i * 2 + 1] = p->vertex[1];
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Trail::BindCamera(Program* program)
{
	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	program->BindUniformFloat4x4(0, proj.ptr(), true);
	program->BindUniformFloat4x4(1, view.ptr(), true);
	program->BindUniformFloat4x4(2, float4x4::identity.ptr(), true);
}
