#pragma once

#include "EditorWindow.h"

#include <memory>

class Model;

class WindowProperties : public EditorWindow
{
public:
	WindowProperties();
	~WindowProperties();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void DrawTransformationTable();
	void DrawGeometryTable();
	void DrawTextureTable();

	std::weak_ptr<Model> model;
};

inline ImVec2 WindowProperties::GetStartingSize() const
{
	return ImVec2(900, 250);
}
