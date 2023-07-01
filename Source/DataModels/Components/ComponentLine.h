#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"

class GameObject;

class ComponentLine : public Component, public Updatable, public Drawable
{
public:
	ComponentLine(bool active, GameObject* owner);
	~ComponentLine() override;

	void Update() override;
	void Draw() const override;

	void InternalSave(Json& meta) override{};
	void InternalLoad(const Json& meta) override{};

	void RecalculateVertices();
	void FaceCamera();

	int GetNumTiles();
	void SetNumTiles(int numTiles);

private:
	void LoadVBO();
	unsigned int quadVBO;
	unsigned int quadVAO;
	int numTiles = 1;

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