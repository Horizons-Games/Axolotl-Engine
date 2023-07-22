#pragma once
#include "GL/glew.h"

class ImGradient;
class ResourceTexture;
class Program;

enum class BlendingMode;

struct Point
{
	float3 centerPosition;
	Quat rotation;
	float life;
};

struct Vertex
{
	float3 position;
	float4 color;
	float2 uv;
};

class Trail
{
public:
	Trail();
	~Trail();

	void Update(float3 newPosition, Quat newRotation);
	void Draw();
	void CreateBuffers();

	bool IsOnPlay();
	void SetOnPlay(bool isRendering);
	
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
	
	void BindCamera(Program* program);

	GLuint vao;
	GLuint vbo;
	GLuint ebo;

	std::vector<Point> points;

	BlendingMode blendingMode;

	bool onPlay;

	int maxSamplers;

	// generation properties
	float duration;
	float minDistance;
	float width;

	// render properties
	ImGradient* gradient;

	std::shared_ptr<ResourceTexture> texture;
};

inline bool Trail::IsOnPlay()
{
	return onPlay;
}

inline void Trail::SetOnPlay(bool onPlay)
{
	this->onPlay = onPlay;
}

inline const float Trail::GetDuration() const
{
	return duration;
}

inline void Trail::SetDuration(float duration)
{
	this->duration = duration;
	points.clear();
}

inline const float Trail::GetMinDistance() const
{
	return minDistance;
}

inline void Trail::SetMinDistance(float minDistance)
{
	this->minDistance = minDistance;
	points.clear();
}

inline const float Trail::GetWidth() const
{
	return width;
}

inline void Trail::SetWidth(float width)
{
	this->width = width;
	points.clear();
}

inline std::shared_ptr<ResourceTexture> Trail::GetTexture() const
{
	return texture;
}

inline void Trail::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->texture = texture;
	points.clear();
}

inline ImGradient* Trail::GetGradient()
{
	return gradient;
}

inline void Trail::SetGradient(ImGradient* gradient)
{
	this->gradient = gradient;
	points.clear();
}