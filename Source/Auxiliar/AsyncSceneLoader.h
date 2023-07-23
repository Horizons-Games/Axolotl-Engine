#pragma once

namespace loader
{
void LoadSceneAsync(const std::string& name, std::function<void(void)> callback, bool mantainCurrentScene);

bool IsLoading();
} // namespace loader
