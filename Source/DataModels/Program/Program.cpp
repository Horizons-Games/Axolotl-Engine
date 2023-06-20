#include "StdAfx.h"

#include "Program.h"

#include "GL/glew.h"

#include <MathGeoLib/Include/Math/float3.h>
#include "SDL/include/SDL.h"

Program::Program(unsigned vertexShader,
				 unsigned fragmentShader,
				 const std::string& vtxShaderFileName,
				 const std::string& frgShaderFileName,
				 const std::string& programName) :
	vertexShaderFileName(vtxShaderFileName),
	fragmentShaderFileName(frgShaderFileName),
	programName(programName),
	id(glCreateProgram())
{
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	int res;
	glGetProgramiv(id, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = (char*) malloc(len);
			glGetProgramInfoLog(id, len, &written, info);
			LOG_INFO("Program Log Info: {}", info);
			free(info);
		}
		SDL_assert(SDL_FALSE && "Problem compiling shaders, read console");
		id = 0;
	}
}

Program::~Program()
{
	CleanUp();
}
