#pragma once
#include "GL/glew.h"

class ImGradient;
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
	
	const float GetDuration() const;
	void SetDuration(float duration);

	const float GetMinDistance() const;
	void SetMinDistance(float minDistance);

	const float GetWidth() const;
	void SetWidth(float width);

	std::shared_ptr<ResourceTexture> GetTexture() const;
	void SetTexture(const std::shared_ptr<ResourceTexture>& texture);

	ImGradient* GetGradient();
	void SetGradient(ImGradient* gradient);

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

	int maxSamplers;

	// generation properties
	float duration;
	float minDistance;
	float width;

	// render properties
	ImGradient* gradient;

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

inline const float Trail::GetDuration() const
{
	return duration;
}

inline void Trail::SetDuration(float duration)
{
	this->duration = duration;
	redoBuffers = true;
}

inline const float Trail::GetMinDistance() const
{
	return minDistance;
}

inline void Trail::SetMinDistance(float minDistance)
{
	this->minDistance = minDistance;
}

inline const float Trail::GetWidth() const
{
	return width;
}

inline void Trail::SetWidth(float width)
{
	this->width = width;
}

inline std::shared_ptr<ResourceTexture> Trail::GetTexture() const
{
	return texture;
}

inline void Trail::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->texture = texture;
	redoBuffers = true;
}

inline ImGradient* Trail::GetGradient()
{
	return gradient;
}

inline void Trail::SetGradient(ImGradient* gradient)
{
	this->gradient = gradient;
	redoBuffers = true;
}