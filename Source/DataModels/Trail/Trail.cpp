#include "StdAfx.h"
#include "Trail.h"

#include "Application.h"

Trail::Trail()
{

}

Trail::~Trail()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}

void Trail::Update(float3 newPosition)
{
	if (!isRendering) // if we are not rendering the tail don't store
	{
		return;
	}
	float3 xAxis = currentPosition;
	for (auto point : points)
	{

	}
	
	UpdateLife();
}

void Trail::CreateBuffers()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * (numTiles * 2 + 2), nullptr, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &uv);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Trail::UpdateLife()
{
	for (auto point : points)
	{
		point->life -= App->GetDeltaTime();
		if (point->life <= 0)
		{
			std::erase(points, point);
		}
	}
}
