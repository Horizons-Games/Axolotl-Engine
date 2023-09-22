#pragma once

#include "GL/glew.h"

#include "FileSystem/UID.h"

#include "Resources/ResourceMesh.h"

struct par_shapes_mesh_s;

class Program;

class LightProxy
{

public:
	LightProxy();
	~LightProxy();

	void DrawLights(Program* program);
	void DrawPoints(Program* program);
	void DrawSpots(Program* program);
	void DrawSpheres(Program* program);
	void DrawTubes(Program* program);

	void LoadShape(par_shapes_mesh_s* shape, ResourceMesh* mesh);

	void SphereShape(float size, unsigned slices, unsigned stacks);
	void ConeShape(float height, float radius, unsigned slices, unsigned stacks);
	void TubeShape(float height, float radius, unsigned slices, unsigned stacks);
	void PlaneShape(float height, float radius, unsigned slices, unsigned stacks);

private:

	int numPointLight;
	int numSpotLight;

	ResourceMesh* sphere;
	ResourceMesh* cone;
	ResourceMesh* tube;
	ResourceMesh* plane;
};

