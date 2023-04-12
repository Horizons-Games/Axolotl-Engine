#pragma once

#include "Resource.h"
#include "IScript.h"
#include <memory>

class ResourceScript;

class ResourceScript : virtual public Resource
{
public:
	ResourceScript(UID resourceUID,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	virtual ~ResourceScript() override;

	ResourceType GetType() const override;

	const char* GetScriptPath();
	ObjectId* GetScriptId();

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	void SaveLoadOptions(Json& meta) override {};
	void LoadLoadOptions(Json& meta) override {};
private:
	const char* scriptPath;
	ObjectId* scriptId;
};

inline ResourceType ResourceScript::GetType() const
{
	return ResourceType::Script;
}

inline const char* ResourceScript::GetScriptPath() 
{
	return scriptPath;
}

inline ObjectId* ResourceScript::GetScriptId()
{
	return scriptId;
}