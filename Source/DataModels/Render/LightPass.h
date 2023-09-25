#pragma once

#include "GL/glew.h"

#include "FileSystem/UID.h"

#include "Resources/ResourceMesh.h"

#include <map>

struct par_shapes_mesh_s;

class ComponentAreaLight;
class ComponentPointLight;
class ComponentSpotLight;
class Program;
class Scene;
class GBuffer;

class LightPass
{

public:
	LightPass();
	~LightPass();

	void CleanUp();

	void DrawLights(Program* program, GBuffer* gbuffer, int renderMode,
					std::vector<ComponentPointLight*> pointsToRender,
					std::vector<ComponentSpotLight*> spotsToRender,
					std::vector<ComponentAreaLight*> spheresToRender,
					std::vector<ComponentAreaLight*> tubesToRender);

	void DrawPoints(Program* program, std::vector<ComponentPointLight*>& pointsToRender, Scene* scene);
	void DrawSpots(Program* program, std::vector<ComponentSpotLight*> spotsToRender, Scene* scene);
	void DrawSpheres(Program* program, std::vector<ComponentAreaLight*>& spheresToRender, Scene* scene);
	void DrawTubes(Program* program, std::vector<ComponentAreaLight*>& tubesToRender, Scene* scene);

	void LoadShape(par_shapes_mesh_s* shape, ResourceMesh* mesh);

	ResourceMesh* CreateSphereShape(float size, unsigned slices, unsigned stacks);
	ResourceMesh* CreateConeShape(float height, float radius, unsigned slices, unsigned stacks);
	ResourceMesh* CreateTubeShape(float height, float radius, unsigned slices, unsigned stacks);
	void CreatePlaneShape(float height, float radius, unsigned slices, unsigned stacks);

	void ReloadSphereShape(ResourceMesh* sphere, float size, unsigned slices, unsigned stacks);
	void ReloadConeShape(ResourceMesh* cone, float height, float radius, unsigned slices, unsigned stacks);
	void ReloadTubeShape(ResourceMesh* tube, float height, float radius, unsigned slices, unsigned stacks);

	void SetScreenSize(unsigned width, unsigned height);

private:

	int numLights;

	std::pair<unsigned, unsigned> screenSize;

	std::unordered_map<const ComponentPointLight*, ResourceMesh*> points;
	std::unordered_map<const ComponentSpotLight*, ResourceMesh*> spots;
	std::unordered_map<const ComponentAreaLight*, ResourceMesh*> spheres;
	std::unordered_map<const ComponentAreaLight*, ResourceMesh*> tubes;
};

inline void LightPass::SetScreenSize(unsigned width, unsigned height)
{
	screenSize.first = width;
	screenSize.second = height;
}

