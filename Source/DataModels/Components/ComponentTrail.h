#pragma once
#include "Component.h"

#include "Auxiliar/Generics/Updatable.h"
#include "Auxiliar/Generics/Drawable.h"
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

struct Curve
{
	float3 a;
	float3 b;
	float3 c;
	float3 d;
};

class ComponentTrail : public Component, public Updatable, public Drawable
{
public:
	ComponentTrail(bool active, GameObject* owner);
	~ComponentTrail();

	void Update() override;
	void Render() override;
	void Draw() const override;

	const float GetDuration() const;
	void SetDuration(float duration);

	const float GetMinDistance() const;
	void SetMinDistance(float minDistance);

	const float GetWidth() const;
	void SetWidth(float width);

	const int GetCatmunPoints();
	void SetCatmunPoints(int numSamplers);

	std::shared_ptr<ResourceTexture> GetTexture() const;
	void SetTexture(const std::shared_ptr<ResourceTexture>& texture);

	ImGradient* GetGradient();
	void SetGradient(ImGradient* gradient);
	
	bool IsOnPlay();
	void SetOnPlay(bool onPlay);

	BlendingMode GetBlendingMode();
	void SetBlendingMode(BlendingMode mode);

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void CreateBuffers();
	void RedoBuffers();

	void UpdateLife();
	bool CheckDistance(float3 comparedPosition);

	void InsertPoint(float3 position, Quat rotation);

	void CalculateExtraPoints(float3& p0, const Point& p1, const Point& p2, float3& p3);

	const Curve& CatmullRomCentripetal(float3 p0, float3 p1, float3 p2, float3 p3);

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
	int catmunPoints;

	// render properties
	ImGradient* gradient;

	std::shared_ptr<ResourceTexture> texture;
};

inline const float ComponentTrail::GetDuration() const
{
	return duration;
}

inline void ComponentTrail::SetDuration(float duration)
{
	this->duration = duration;
	points.clear();
}

inline const float ComponentTrail::GetMinDistance() const
{
	return minDistance;
}

inline void ComponentTrail::SetMinDistance(float minDistance)
{
	this->minDistance = minDistance;
	points.clear();
}

inline const float ComponentTrail::GetWidth() const
{
	return width;
}

inline void ComponentTrail::SetWidth(float width)
{
	this->width = width;
	points.clear();
}

inline const int ComponentTrail::GetCatmunPoints()
{
	return catmunPoints;
}

inline void ComponentTrail::SetCatmunPoints(int numSamplers)
{
	catmunPoints = numSamplers;
	points.clear();
}

inline std::shared_ptr<ResourceTexture> ComponentTrail::GetTexture() const
{
	return texture;
}

inline void ComponentTrail::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->texture = texture;
	points.clear();
}

inline ImGradient* ComponentTrail::GetGradient()
{
	return gradient;
}

inline void ComponentTrail::SetGradient(ImGradient* gradient)
{
	this->gradient = gradient;
	points.clear();
}

inline bool ComponentTrail::IsOnPlay()
{
	return onPlay;
}

inline void ComponentTrail::SetOnPlay(bool onPlay)
{
	this->onPlay = onPlay;
	points.clear();
}

inline BlendingMode ComponentTrail::GetBlendingMode()
{
	return blendingMode;
}

inline void ComponentTrail::SetBlendingMode(BlendingMode mode)
{
	blendingMode = mode;
	points.clear();
}
