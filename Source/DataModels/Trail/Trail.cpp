#include "StdAfx.h"
#include "Trail.h"

#include "Application.h"

Trail::Trail()
{
	glGenBuffers(1, &vbo);
}

Trail::~Trail()
{
}

void Trail::Update(float3 position)
{

	for (auto point : points)
	{

	}
	
	UpdateLife();
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
