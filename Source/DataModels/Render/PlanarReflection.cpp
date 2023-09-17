#include "StdAfx.h"
#include "PlanarReflection.h"

PlanarReflection::PlanarReflection() : frameBuffer(0), depth(0), reflectionTex(0)
{
}

PlanarReflection::~PlanarReflection()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &depth);

	glDeleteTextures(1, &reflectionTex);
}

void PlanarReflection::InitBuffer(unsigned width, unsigned height)
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

	glGenTextures(1, &reflectionTex);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer Reflection is not complete!");
	}
}
