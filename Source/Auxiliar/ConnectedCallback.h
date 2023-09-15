#pragma once

class [[nodiscard]] ConnectedCallback
{
public:
	ConnectedCallback(std::function<void(void)>&& deregisterMethod);
	~ConnectedCallback();

private:
	std::function<void(void)> deregisterMethod;
};
