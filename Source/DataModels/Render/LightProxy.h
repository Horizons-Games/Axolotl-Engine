#pragma once
#include "GL/glew.h"
#include "FileSystem/UID.h"
#include "Resources/ResourceMesh.h"

class Program;

class LightProxy
{

public:
	LightProxy();
	~LightProxy();

	void DrawAreaLights(Program* program, GLuint frameBuffer);

	void SphereShape(float size, unsigned slices, unsigned stacks);
	void ConeShape(float height, float radius, unsigned slices, unsigned stacks);
	void CylinderShape(float height, float radius, unsigned slices, unsigned stacks);


private:

	int numPointLight;
	int numSpotLight;

	ResourceMesh* sphere;
	ResourceMesh* cone;
	ResourceMesh* cylinder;
};

