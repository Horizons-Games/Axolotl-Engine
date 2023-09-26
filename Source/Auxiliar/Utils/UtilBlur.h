#pragma once

#include "GL/glew.h"

class UtilBlur
{
public:
	UtilBlur();
	~UtilBlur();

	void BlurTexture(const GLuint inputTex, const GLuint output, GLuint internalFormat,	GLuint format, GLuint type, 
		unsigned int inMip, unsigned int inWidth, unsigned int inHeight, unsigned int outMip, unsigned int outWidth, 
		unsigned int outHeight);

private:

	void GenerateTexture(GLuint nInternalFormat, GLuint nFormat, GLuint nType, GLuint nWidth, GLuint nHeight);

private:

	GLuint horizontalFrameBuffer = 0;
	GLuint verticalFrameBuffer = 0;

	GLuint auxTex = 0;

	// Texture Properties
	GLuint internalFormat = 0;
	GLuint format = 0;
	GLuint type = 0;
	GLuint width = 0;
	GLuint height = 0;
};
