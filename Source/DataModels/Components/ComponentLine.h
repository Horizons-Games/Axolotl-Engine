#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"
#include "Program/Program.h"

#include "Math/float2.h"
#include "Math/float3.h"

class GameObject;

class ComponentLine : public Component, public Drawable
{
public:
	ComponentLine(bool active, GameObject* owner);
	~ComponentLine() override;

	void Draw() const override;

	void Render();

	void ModelMatrix(Program* program);

	void InternalSave(Json& meta) override{};
	void InternalLoad(const Json& meta) override{};

	void RecalculateVertices();
	void FaceCamera();

	int GetNumTiles();
	void SetNumTiles(int numTiles);

private:

	void UpdateBuffers() const;
	void LoadBuffers();

	unsigned int positionBuffers;
	unsigned int textureBuffers;
	unsigned int colorBuffers;
	unsigned int quadVAO;
	unsigned int quadEBO;
	int numTiles = 1;

	bool dirtyBuffers = true;

	GameObject* childGameObject;
};

inline int ComponentLine::GetNumTiles()
{
	return numTiles;
}

inline void ComponentLine::SetNumTiles(int numTiles)
{
	this->numTiles = numTiles;
	RecalculateVertices();
}