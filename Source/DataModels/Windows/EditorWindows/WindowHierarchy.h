#pragma once
#include "EditorWindow.h"

class GameObject;

class WindowHierarchy : public EditorWindow
{
public:
	WindowHierarchy();
	~WindowHierarchy() override;

protected:
	void DrawWindowContents() override;

private:
	void DrawRecursiveHierarchy(GameObject* gameObject);

    void Create2DObjectMenu(GameObject* gameObject);

    void MoveObjectMenu(GameObject* gameObject, std::vector<GameObject*>& children);

    void Create3DObjectMenu(GameObject* gameObject);

	bool IsModifiable(const GameObject* gameObject) const;
    void DeleteGameObject(const GameObject* gameObject) const;

	void CopyAnObject();
	void PasteAnObject();
	void CutAnObject();
	void DuplicateAnObject();

	std::unique_ptr<GameObject> copyObject;
	bool objectHasBeenCut;
};
