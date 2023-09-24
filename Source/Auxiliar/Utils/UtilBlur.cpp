#include "StdAfx.h"
#include "UtilBlur.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "Program/Program.h"

UtilBlur* UtilBlur::instanceBlur;

UtilBlur::~UtilBlur()
{
	glDeleteFramebuffers(2, blurFrameBuffer);
}

UtilBlur* UtilBlur::GetInstanceBlur()
{
	if (!instanceBlur)
	{
		instanceBlur = new UtilBlur();
	}
	return instanceBlur;
}

void UtilBlur::BlurTexture(GLuint inputTex, GLuint auxTex, int w, int h, int mipmapLevel)
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::GAUSSIAN_BLUR);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("SSAO - Gaussian Blur")),
		"SSAO - Gaussian Blur");

	glViewport(0, 0, w, h);

	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffer[0]);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, auxTex, mipmapLevel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputTex);

	program->BindUniformFloat2("invSize", float2(1.0f / w, 1.0f / h));
	program->BindUniformFloat2("blurDirection", float2(1.0f, 0.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffer[1]);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inputTex, mipmapLevel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, auxTex);

	program->BindUniformFloat2("blurDirection", float2(0.0f, 1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	program->Deactivate();

	glPopDebugGroup();
}

void UtilBlur::BlurTexture(GLuint inputTex, GLuint auxTex, GLuint outputTex, int w, int h, int mipmapLevel)
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::GAUSSIAN_BLUR);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("SSAO - Gaussian Blur")),
		"SSAO - Gaussian Blur");

	glViewport(0, 0, w, h);
	
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffer[0]);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, auxTex, mipmapLevel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputTex);

	program->BindUniformFloat2("invSize", float2(1.0f / w, 1.0f / h));
	program->BindUniformFloat2("blurDirection", float2(1.0f, 0.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBuffer[1]);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex, mipmapLevel);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, auxTex);

	program->BindUniformFloat2("blurDirection", float2(0.0f, 1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	program->Deactivate();

	glPopDebugGroup();
}

UtilBlur::UtilBlur()
{
	InitBuffer();
}

void UtilBlur::InitBuffer()
{
	glGenFramebuffers(2, blurFrameBuffer);
}
