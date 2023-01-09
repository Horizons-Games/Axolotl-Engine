#pragma once
class Skybox
{

public:
	Skybox();
	void Draw();

private:
	unsigned vbo;
	unsigned vao;
	unsigned texture;

};

