#pragma once

#include "Resources/Resource.h"

class EditorResourceInterface : virtual public Resource
{
public:
	virtual ~EditorResourceInterface() override;

	virtual bool ToDelete() const = 0;

	virtual void MarkToDelete() = 0;
};

inline EditorResourceInterface::~EditorResourceInterface()
{
}
