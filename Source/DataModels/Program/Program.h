#pragma once
class Program
{

public:
	Program(unsigned vertexShader, unsigned fragmentShader);
	~Program();

	void Activate();
	void Deactivate();
	void CleanUp();
	void BindUniformFloat4x4(const char* name, const float* data, bool transpose);
	void BindUniformFloat3(const char* name, const float* data);
	void BindUniformFloat(const char* name, const float* data);
	void BindUniformBool(const char* name, bool value);

	const unsigned& GetId() const;

private:
	unsigned id;
};

inline const unsigned& Program::GetId() const
{
	return id;
}
