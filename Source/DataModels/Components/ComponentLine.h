#pragma once

#include "Components/Component.h"

#include "ImGui/imgui_color_gradient.h"
#include "ImGui/imgui_bezier.h"


#include "Timer/Timer.h"

class GameObject;
class ResourceTexture;
class Program;

class ComponentLine : public Component
{
public:
	ComponentLine(bool active, GameObject* owner);
	~ComponentLine() override;

	void Render();

	void ModelMatrix(Program* program);

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	const GameObject* GetEnd() const;
	void SetEnd(GameObject* end);

	const int GetNumTiles() const;
	void SetNumTiles(int numTiles);

	const float GetSpeed() const;
	void SetSpeed(float speed);

	const float GetTime() const;
	void SetTime(float time);

	const float2& GetTiling() const;
	void SetTiling(float2& tiling);

	const float2& GetOffset() const;
	void SetOffset(const float2& offset);

	const float2& GetSizeFading() const;
	void  SetSizeFading(const float2& range);

	const float4& GetSizeFadingPoints() const;
	void  SetSizeFadingPoints(const float4& points);

	std::shared_ptr<ResourceTexture> GetLineTexture() const;
	void SetLineTexture(const std::shared_ptr<ResourceTexture>& texture);

	ImGradient* GetGradient();
	void SetGradient(ImGradient* gradient);

private:
	void UpdateBuffers();
	void LoadBuffers();

	unsigned int positionBuffers;
	unsigned int textureBuffers;
	unsigned int colorBuffers;
	unsigned int lineEBO;
	unsigned int lineVAO;
	int numTiles;

	bool dirtyBuffers;

	GameObject* childGameObject;


	std::shared_ptr<ResourceTexture> lineTexture;

	float speed;
	float time;
	float2 offset;
	float2 tiling;
	ImGradient* gradient;
	float2 sizeFading;
	float4 sizeFadingPoints;
};

inline const int ComponentLine::GetNumTiles() const
{
	return numTiles;
}

inline void ComponentLine::SetNumTiles(int numTiles)
{
	this->numTiles = numTiles;
	dirtyBuffers = true;
}

inline const float ComponentLine::GetSpeed() const
{
	return speed;
}

inline void ComponentLine::SetSpeed(float speed)
{
	this->speed = speed;
}

inline const float ComponentLine::GetTime() const
{
	return time;
}

inline void ComponentLine::SetTime(float time)
{
	this->time += time;
}

inline const float2& ComponentLine::GetTiling() const
{
	return tiling;
}

inline void ComponentLine::SetTiling(float2& tiling)
{
	this->tiling = tiling;
}

inline const float2& ComponentLine::GetOffset() const
{
	return offset;
}

inline void ComponentLine::SetOffset(const float2& offset)
{
	this->offset = offset;
}

inline std::shared_ptr<ResourceTexture> ComponentLine::GetLineTexture() const
{
	return lineTexture;
}

inline void ComponentLine::SetLineTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	lineTexture = texture;
	dirtyBuffers = true;
}

inline ImGradient* ComponentLine::GetGradient()
{
	return gradient;
}

inline void ComponentLine::SetGradient(ImGradient* gradient)
{
	this->gradient = gradient;
	dirtyBuffers = true;
}

inline const float2& ComponentLine::GetSizeFading() const
{
	return sizeFading;
}
inline void ComponentLine::SetSizeFading(const float2& range)
{
	sizeFading = range;
	dirtyBuffers = true;
}

inline const float4& ComponentLine::GetSizeFadingPoints() const
{
	return sizeFadingPoints;
}
inline void ComponentLine::SetSizeFadingPoints(const float4& points)
{
	sizeFadingPoints = points;
	dirtyBuffers = true;
}