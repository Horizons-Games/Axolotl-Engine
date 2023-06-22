#pragma once

class Window
{
public:
	virtual ~Window()
	{
	}

	virtual void Draw(bool& enabled) = 0;

	const std::string& GetName() const;

protected:
	Window(const std::string& name);
	std::string name;
};

inline Window::Window(const std::string& name)
{
	this->name = name;
}

inline const std::string& Window::GetName() const
{
	return name;
}
