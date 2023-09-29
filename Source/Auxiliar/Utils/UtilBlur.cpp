#include "StdAfx.h"
#include "UtilBlur.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "Program/Program.h"

UtilBlur::UtilBlur()
{
}

UtilBlur::~UtilBlur()
{
	glDeleteFramebuffers(1, &horizontalFrameBuffer);
	glDeleteFramebuffers(1, &verticalFrameBuffer);

	glDeleteTextures(1, &auxTex);
}

void UtilBlur::BlurTexture(const GLuint inputTex, const GLuint outputTex, GLuint internalFormat, GLuint format, 
	GLuint type, unsigned int inMip, unsigned int inWidth, unsigned int inHeight, unsigned int outMip,
	unsigned int outWidth, unsigned int outHeight)
{
	if (horizontalFrameBuffer == 0)
	{
		glGenFramebuffers(1, &horizontalFrameBuffer);
	}

	if (verticalFrameBuffer == 0)
	{
		glGenFramebuffers(1, &verticalFrameBuffer);
	}

	GenerateTexture(internalFormat, format, type, outWidth, outHeight);

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::GAUSSIAN_BLUR);
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, horizontalFrameBuffer);
	glViewport(0, 0, outWidth, outHeight);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, auxTex, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputTex);

	program->BindUniformFloat2("invSize", float2(1.0f / inWidth, 1.0f / inHeight));
	program->BindUniformFloat2("blurDirection", float2(1.0f, 0.0f));
	program->BindUniformInt(0, inMip);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, verticalFrameBuffer);
	glViewport(0, 0, outWidth, outHeight);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex, outMip);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, auxTex);

	program->BindUniformFloat2("blurDirection", float2(0.0f, 1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);
	program->Deactivate();
}

void UtilBlur::GenerateTexture(GLuint nInternalFormat, GLuint nFormat, GLuint nType, GLuint nWidth, GLuint nHeight)
{
	if (auxTex == 0)
	{
		glGenTextures(1, &auxTex);
	}

	if (internalFormat != nInternalFormat || format != nFormat || type != nType || width != nWidth || height != nHeight)
	{
		glBindTexture(GL_TEXTURE_2D, auxTex);
		glTexImage2D(GL_TEXTURE_2D, 0, nInternalFormat, nWidth, nHeight, 0, nFormat, nType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		/*GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			SDL_assert(SDL_FALSE && "Problem auxTexture Blur");
		}*/

		internalFormat = nInternalFormat;
		format = nFormat;
		type = nType;
		width = nWidth;
		height = nHeight;
	}
}
