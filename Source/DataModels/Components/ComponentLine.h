#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"
#include "Program/Program.h"

#include "Math/float2.h"
#include "Math/float3.h"
#include "ImGui/imgui_color_gradient.h"

class GameObject;
class ResourceTexture;

class ComponentLine : public Component
{
public:
	ComponentLine(bool active, GameObject* owner);
	~ComponentLine() override;

	void Render();

	void ModelMatrix(Program* program);

	void InternalSave(Json& meta) override{};
	void InternalLoad(const Json& meta) override{};

	void RecalculateVertices();

	int GetNumTiles();
	void SetNumTiles(int numTiles);

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
	int numTiles = 1;

	bool dirtyBuffers = true;

	GameObject* childGameObject;


	std::shared_ptr<ResourceTexture> lineTexture;

	float2 offset = float2::zero;
	float2 tiling = float2::one;
	ImGradient* gradient = new ImGradient();
};

inline int ComponentLine::GetNumTiles()
{
	return numTiles;
}

inline void ComponentLine::SetNumTiles(int numTiles)
{
	this->numTiles = numTiles;
	dirtyBuffers = true;
}

inline std::shared_ptr<ResourceTexture> ComponentLine::GetLineTexture() const
{
	return lineTexture;
}

inline void ComponentLine::SetLineTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->lineTexture = texture;
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