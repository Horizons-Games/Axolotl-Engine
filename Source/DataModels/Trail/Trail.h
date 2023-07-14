#pragma once
#include "GL/glew.h"

class ImGradient;
class ImGradientMark;
class ResourceTexture;
class Program;

struct Point
{
	float3 centerPosition;
	float3 vertex[2];
	float life;
};

class Trail
{
public:
	Trail();
	~Trail();

	void Update(float3 newPosition, Quat newRotation);
	void Draw();
	void CreateBuffers();

	bool IsRendering();
	void SetRendering(bool isRendering);
	
private:
	void RedoBuffers();

	void UpdateLife();
	bool CheckDistance(float3 comparedPosition);

	void InsertPoint(float3 position, Quat rotation);
	void CalculateUV();
	void CalculateVBO();

	void BindCamera(Program* program);

	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint uv;

	float3 lastPosition;

	std::vector<std::unique_ptr<Point>> points;

	bool isRendering;

	int maxPoints;

	// generation properties
	float duration;
	float minDistance;
	float widht;

	// render properties
	ImGradient* gradient;
	ImGradientMark* draggingMark;
	ImGradientMark* selectedMark;

	bool redoBuffers;

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
