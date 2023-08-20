#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Updatable.h"
#include "GL/glew.h"

class Program;

class ComponentLightProbe : public Component, public Updatable
{
public:
	ComponentLightProbe(bool active, GameObject* owner);
	~ComponentLightProbe() override;

	void Update() override;

	const uint64_t& GetHandle();

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	void BindCameraToProgram(Program* program, Frustum frustum);

	GLuint frameBuffer;
	GLuint lightProbe;

	uint64_t handle;
};