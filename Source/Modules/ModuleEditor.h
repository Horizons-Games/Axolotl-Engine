#pragma once
#include "Module.h"

class EditorWindow;
class WindowMainMenu;
class WindowDebug;
class WindowScene;
class WindowInspector;
class Resource;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor() override;

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	void Resized();

	const WindowScene* GetScene() const;

	bool IsSceneFocused() const;
	void SetResourceOnInspector(const std::weak_ptr<Resource>& resource) const;

private:
//to be clear: this is a bad idea and can lead to compiler errors, but at this point it's the least of my worries
#ifdef ENGINE
	std::vector<std::unique_ptr<EditorWindow> > windows;
	std::unique_ptr<WindowMainMenu> mainMenu = nullptr;
#endif
	std::unique_ptr<WindowDebug> debugOptions = nullptr;

	WindowInspector* inspector;
	WindowScene* scene;
	bool windowResized;
};

inline const WindowScene* ModuleEditor::GetScene() const
{
	return scene;
}