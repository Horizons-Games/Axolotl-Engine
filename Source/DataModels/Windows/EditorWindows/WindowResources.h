#pragma once

#include "EditorWindow.h"

class Resource;
class EditorResourceInterface;

class WindowResources : public EditorWindow
{
public:
	WindowResources();
	~WindowResources() = default;

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void DrawResource(const std::weak_ptr<Resource>& resource,
					  std::vector<std::shared_ptr<EditorResourceInterface>>& resourcesToDelete);
};

inline WindowResources::WindowResources() : EditorWindow("Resources")
{
}

inline ImVec2 WindowResources::GetStartingSize() const
{
	return ImVec2();
}
