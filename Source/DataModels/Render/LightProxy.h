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

	void SphereShape(const char* name, float size, unsigned slices, unsigned stacks, UID uid);
	void ConeShape(const char* name, float height, float radius, unsigned slices, unsigned stacks, UID uid);
	void CylinderShape(const char* name, float height, float radius, unsigned slices, unsigned stacks,
		UID uid);


private:

	//friend class ModuleResources;

	int numPointLight;
	int numSpotLight ;
	//ModuleResources* resources;

	ResourceMesh* sphere = nullptr;
	ResourceMesh* cone = nullptr;
	ResourceMesh* cylinder = nullptr;
};

