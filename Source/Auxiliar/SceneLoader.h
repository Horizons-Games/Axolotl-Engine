#pragma once

#include "rapidjson/document.h"

namespace loader
{
enum class LoadMode
{
	BLOCKING,
	ASYNCHRONOUS
};

void LoadScene(std::variant<std::string, std::reference_wrapper<rapidjson::Document>> sceneNameOrDocument,
			   std::function<void(void)> callback,
			   bool mantainCurrentScene,
			   LoadMode loadMode);

bool IsLoading();
} // namespace loader
