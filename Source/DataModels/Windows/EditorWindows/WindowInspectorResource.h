#pragma once

#include "EditorWindow.h"

#include "FileSystem/UniqueID.h"
#include <memory>

class Resource;

class WindowInspectorResource : public EditorWindow
{
public:
	WindowInspectorResource();
	~WindowInspectorResource();

	void SetResource(const std::weak_ptr<Resource>& resource);

protected:
	void DrawWindowContents() override;
	void DrawTextureOptions();
	ImVec2 GetStartingSize() const override;

private:
	bool MousePosIsInWindow();
	bool WindowRightClick();

	void InitTextureImportOptions();

	std::weak_ptr<Resource> resource;

	//Import Options (Move this to another class? Probably)
	//Texture
	bool flipVertical;
	bool flipHorizontal;
};

inline ImVec2 WindowInspectorResource::GetStartingSize() const
{
	return ImVec2(900, 250);
}
