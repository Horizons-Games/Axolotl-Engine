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

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	void BindCameraToProgram(Program* program, GLuint uboCamera, Frustum frustum);

	GLuint frameBuffer;
	GLuint preFiltered;
};