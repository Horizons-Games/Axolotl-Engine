#pragma once
#include "GL/glew.h"

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

	void Update(float3 newPosition);
	void CreateBuffers();

	bool IsRendering();
	void SetRendering(bool isRendering);
	
private:
	void UpdateLife();

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint uv;

	float3 currentPosition;
	float3 oldPosition;

	std::vector<Point*> points;

	bool isRendering;

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

inline bool Trail::IsRendering()
{
	return isRendering;
}

inline void Trail::SetRendering(bool isRendering)
{
	this->isRendering = isRendering;
}
