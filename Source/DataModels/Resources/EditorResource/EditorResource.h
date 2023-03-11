#pragma once
#pragma warning (disable: 4250)

#include <string>

#include "Resources/EditorResource/EditorResourceInterface.h"
#include "Resources/Resource.h"

//ideally this template should be safeguarded like in GameObject::GetComponentByType
//but I had some issues and I don't want to waste time on it
template<typename R = Resource>
class EditorResource : public R, public EditorResourceInterface
{
public:
	EditorResource(UID resourceUID,
				   const std::string& fileName,
				   const std::string& assetsPath,
				   const std::string& libraryPath);
	virtual ~EditorResource() override;

	bool ToDelete() const override;

	void MarkToDelete() override;

private:
	bool toDelete;
};

template<typename R>
inline EditorResource<R>::EditorResource(UID resourceUID,
										 const std::string& fileName,
										 const std::string& assetsPath,
										 const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath),
	R(resourceUID, fileName, assetsPath, libraryPath),
	toDelete(false)
{
}

template<typename R>
inline EditorResource<R>::~EditorResource()
{
}

template<typename R>
inline bool EditorResource<R>::ToDelete() const
{
	return toDelete;
}

template<typename R>
inline void EditorResource<R>::MarkToDelete()
{
	toDelete = true;
}
