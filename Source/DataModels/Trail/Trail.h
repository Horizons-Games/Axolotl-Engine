#pragma once
#include "Math/float3.h"
#include "GL/glew.h"

#include <vector>
#include <memory>

class ImGradient;
class ImGradientMark;
class ResourceTexture;

struct Point
{
	float3 position;
	float life;
};

class Trail
{
public:
	Trail();
	~Trail();

	void Update(float3 position);
	
private:
	void UpdateLife();

	GLuint vbo;

	float3 position;

	std::vector<Point*> points;

	// generation properties
	float duration;
	float minDistance;
	float widht;

	// render properties
	ImGradient* gradient;
	ImGradientMark* draggingMark;
	ImGradientMark* selectedMark;

	std::shared_ptr<ResourceTexture> texture;
};

