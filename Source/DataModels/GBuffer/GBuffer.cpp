#include "StdAfx.h"

#include "GBuffer.h"
#include "AxoLog.h"

GBuffer::GBuffer() :
	gFrameBuffer(0),
	gPosition(0),
	gNormal(0),
	gDiffuse(0),
	gSpecular(0),
	gEmissive(0),
	gDepth(0)
{
}

GBuffer::~GBuffer()
{
	glDeleteFramebuffers(1, &gFrameBuffer);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gDiffuse);
	glDeleteTextures(1, &gSpecular);
	glDeleteTextures(1, &gEmissive);
	glDeleteTextures(1, &gDepth);
	//glDeleteRenderbuffers(1, &gDepth);
}

void GBuffer::BindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer);
}

void GBuffer::ReadFrameBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gFrameBuffer);
}

void GBuffer::DrawFrameBuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gFrameBuffer);
}

void GBuffer::UnBindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::BindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gDiffuse);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gSpecular);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gEmissive);
}

void GBuffer::UnBindTexture()
{
}

void GBuffer::InitGBuffer(unsigned width, unsigned height)
{
	glGenFramebuffers(1, &gFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gFrameBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gDiffuse);
	glBindTexture(GL_TEXTURE_2D, gDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDiffuse, 0);

	glGenTextures(1, &gSpecular);
	glBindTexture(GL_TEXTURE_2D, gSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);

	glGenTextures(1, &gEmissive);
	glBindTexture(GL_TEXTURE_2D, gEmissive);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gEmissive, 0);

	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	/*glGenRenderbuffers(1, &gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gDepth);*/

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("G Framebuffer not complete!");
	}
}
