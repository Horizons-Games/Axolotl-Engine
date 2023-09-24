#pragma once

#include "GL/glew.h"

class UtilBlur
{
public:
	~UtilBlur();

	static UtilBlur* GetInstanceBlur();

	UtilBlur(const UtilBlur& obj) = delete;

	void BlurTexture(GLuint inputTex, GLuint auxTex, int w, int h, int mipmapLevel = 0);
	void BlurTexture(GLuint inputTex, GLuint auxTex, GLuint outputTex, int w, int h, int mipmapLevel = 0);

private:
	UtilBlur();
	
	static UtilBlur* instanceBlur;

private:
	void InitBuffer();

	GLuint blurFrameBuffer[2] = { 0, 0 };
};
