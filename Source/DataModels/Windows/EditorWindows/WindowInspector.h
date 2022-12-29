#pragma once
#include "EditorWindow.h"



#include <memory>

class Model;

class WindowInspector : public EditorWindow
{
public:
	WindowInspector();
	~WindowInspector();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;


private:
	void DrawTransformationTable();
	void DrawGeometryTable();
	void DrawTextureTable();
	
	std::weak_ptr<Model> modelInspector;
};

inline ImVec2 WindowInspector::GetStartingSize() const
{
	return ImVec2(900, 250);
}
