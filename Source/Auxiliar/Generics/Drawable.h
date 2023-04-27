#pragma once

class Drawable
{
public:
	virtual ~Drawable() = default;

	virtual void Draw() const = 0;
};
