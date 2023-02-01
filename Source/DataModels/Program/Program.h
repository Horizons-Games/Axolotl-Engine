#pragma once

#include "Math/float3.h"

class Program
{

public:
	Program(unsigned vertexShader, unsigned fragmentShader);
	void Activate();
	void Deactivate();
	void CleanUp();
	void BindUniformFloat4x4(const char* name, const float* data, bool transpose);
	void BindUniformFloat3(const char* name, const float3 data);
	void BindUniformFloat(const char* name, const float data);
	void BindUniformInt(const char* name, int value);
	const unsigned& GetId() const;

private:
	unsigned id;
};

inline const unsigned& Program::GetId() const
{
	return id;
}
