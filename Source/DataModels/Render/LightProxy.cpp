#include "StdAfx.h"

#include "LightProxy.h"

#include "Application.h"

#include "Components/ComponentAreaLight.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleProgram.h"

#include "ParShapes/par_shapes.h"

#include "Program/Program.h"

#include "Scene/Scene.h"

#include "GameObject/GameObject.h"


LightProxy::LightProxy() : numLights(0)
{
}

LightProxy::~LightProxy()
{
	CleanUp();
}

void LightProxy::CleanUp()
{
	for (auto point : points)
	{
		delete point.second;
	}
	for (auto spot : spots)
	{
		delete spot.second;
	}
	for (auto sphere : spheres)
	{
		delete sphere.second;
	}
	for (auto tube : tubes)
	{
		delete tube.second;
	}

	points.clear();
	spots.clear();
	spheres.clear();
	tubes.clear();
}

void LightProxy::DrawLights(Program* program,
							std::vector<ComponentPointLight*> pointsToRender,
							std::vector<ComponentSpotLight*> spotsToRender,
							std::vector<ComponentAreaLight*> spheresToRender)
{
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Light Culling"), "Light Culling");

	program->BindUniformFloat2("screenSize", float2(screenSize.first, screenSize.second));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	Scene* scene = App->GetModule<ModuleScene>()->GetLoadedScene();

	DrawPoints(program, pointsToRender, scene);
	DrawSpots(program, spotsToRender, scene);
	DrawSpheres(program, spheresToRender, scene);
	//DrawTubes(program); //No worth to draw, needed too many welds to get a tube mesh

	glFrontFace(GL_CW);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glPopDebugGroup();

	program->Deactivate();
}

void LightProxy::DrawPoints(Program* program, std::vector<ComponentPointLight*>& pointsToRender, Scene* scene)
{
	for (ComponentPointLight* point : pointsToRender)
	{
		int index = scene->GetPointIndex(point);
		float radius = point->GetRadius();

		if (points.find(point) == points.end())
		{
			points[point] = CreateSphereShape(radius, 15, 15);
		}
		else if (point->IsDirty())
		{
			ReloadSphereShape(points[point], radius, 15, 15);
			point->SetDirty(false);
		}

		ResourceMesh* mesh = points[point];
		float4x4 transform = point->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		program->BindUniformInt("lightIndex", index);
		program->BindUniformInt("flagPoint", 1);
		program->BindUniformInt("flagSpot", 0);
		program->BindUniformInt("flagSphere", 0);
		program->BindUniformInt("flagTube", 0);

		glBindVertexArray(mesh->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

		glDrawElements(GL_TRIANGLES, mesh->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);		
	}
}

void LightProxy::DrawSpots(Program* program, std::vector<ComponentSpotLight*> spotsToRender, Scene* scene)
{
	for (ComponentSpotLight* spot : spotsToRender)
	{
		float index = scene->GetSpotIndex(spot);
		float height = spot->GetRadius();
		float radius = height * math::Tan(spot->GetOuterAngle());

		if (spots.find(spot) == spots.end())
		{
			spots[spot] = CreateConeShape(height, radius, 15, 15);
		}
		else if (spot->IsDirty())
		{
			ReloadConeShape(spots[spot], height, radius, 15, 15);
			spot->SetDirty(false);
		}

		ResourceMesh* mesh = spots[spot];
		float4x4 transform = spot->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		program->BindUniformInt("lightIndex", index);
		program->BindUniformInt("flagSpot", 1);
		program->BindUniformInt("flagPoint", 0);
		program->BindUniformInt("flagSphere", 0);
		program->BindUniformInt("flagTube", 0);

		glBindVertexArray(mesh->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

		glDrawElements(GL_TRIANGLES, mesh->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

void LightProxy::DrawSpheres(Program* program, std::vector<ComponentAreaLight*>& spheresToRender, Scene* scene)
{
	for (ComponentAreaLight* sphere : spheresToRender)
	{
		int index = scene->GetSphereIndex(sphere);
		float radius = sphere->GetAttRadius() + sphere->GetShapeRadius();

		if (spheres.find(sphere) == spheres.end())
		{
			spheres[sphere] = CreateSphereShape(radius, 15, 15);
		}
		else if (sphere->IsDirty())
		{
			ReloadSphereShape(spheres[sphere], radius, 15, 15);
			sphere->SetDirty(false);
		}

		ResourceMesh* mesh = spheres[sphere];
		float4x4 transform = sphere->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		program->BindUniformInt("lightIndex", index);
		program->BindUniformInt("flagSphere", 1);
		program->BindUniformInt("flagSpot", 0);
		program->BindUniformInt("flagPoint", 0);
		program->BindUniformInt("flagTube", 0);

		glBindVertexArray(mesh->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

		glDrawElements(GL_TRIANGLES, mesh->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

void LightProxy::DrawTubes(Program* program, std::vector<ComponentAreaLight*>& tubesToRender, Scene* scene)
{
	for (ComponentAreaLight* tube : tubesToRender)
	{
		int index = scene->GetTubeIndex(tube);
		float height = tube->GetHeight();
		float radius = tube->GetAttRadius() + tube->GetShapeRadius();

		if (tubes.find(tube) == spheres.end())
		{
			tubes[tube] = CreateTubeShape(height, radius, 15, 5);
		}
		else if (tube->IsDirty())
		{
			ReloadTubeShape(tubes[tube], height, radius, 15, 15);
			tube->SetDirty(false);
		}

		ResourceMesh* mesh = tubes[tube];
		float4x4 transform = tube->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);
		
		program->BindUniformInt("lightIndex", index);
		program->BindUniformInt("flagTube", 1);
		program->BindUniformInt("flagSphere", 0);
		program->BindUniformInt("flagSpot", 0);
		program->BindUniformInt("flagPoint", 0);

		glBindVertexArray(mesh->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

		glDrawElements(GL_TRIANGLES, mesh->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

void LightProxy::LoadShape(par_shapes_mesh_s* shape, ResourceMesh* mesh)
{
	if (mesh->IsLoaded())
	{
		mesh->Unload();
	}

	std::vector<float3> vertices(shape->npoints);
	memcpy(&vertices[0], shape->points, shape->npoints * sizeof(float3));

	mesh->SetVertices(vertices);
	mesh->SetNumVertices(shape->npoints);

	if (shape->normals)
	{
		std::vector<float3> normals(shape->npoints);
		memcpy(&normals[0], shape->normals, shape->npoints * sizeof(float3));
		mesh->SetNormals(normals);
	}

	if (shape->tcoords)
	{
		std::vector<float2> tcoordsCopy(shape->npoints);
		memcpy(&tcoordsCopy[0], shape->tcoords, shape->npoints * sizeof(float2));

		std::vector<float3> tcoords;
		tcoords.reserve(shape->npoints);

		for (int i = 0; i < tcoordsCopy.size(); i++)
		{
			tcoords.push_back(float3(tcoordsCopy[i].x, tcoordsCopy[i].y, 0.0f));
		}

		mesh->SetTextureCoords(tcoords);
	}

	std::vector<unsigned int> facesIndexes(shape->triangles, shape->triangles + shape->ntriangles * 3);
	std::vector<std::vector<unsigned int>> faces;
	for (unsigned int i = 0; i + 2 < shape->ntriangles * 3; i += 3)
	{
		std::vector<unsigned int> indexes{ facesIndexes[i], facesIndexes[i + 1], facesIndexes[i + 2] };
		faces.push_back(indexes);
	}
	mesh->SetFacesIndices(faces);
	mesh->SetNumFaces(shape->ntriangles);
	mesh->SetNumIndexes(shape->ntriangles * 3);

	mesh->Load();
}

ResourceMesh* LightProxy::CreateSphereShape(float size, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, stacks);
	
	ResourceMesh* sphere = nullptr;

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);

		sphere = new ResourceMesh(numLights, "", "", "");
		++numLights;

		LoadShape(mesh, sphere);

		par_shapes_free_mesh(mesh);
	}

	return sphere;
}

ResourceMesh* LightProxy::CreateConeShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cone(slices, stacks);

	ResourceMesh* cone = nullptr;

	if (mesh)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5f), reinterpret_cast<const float*>(&float3::unitX));
		par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, radius);

		const float3 center = float3(0.0f, -(height / 2.0f), 0.0f);
		const float3 normal = -float3::unitY;
		par_shapes_mesh* disk = par_shapes_create_disk(radius, slices, &center.x, &normal.x);

		if (disk)
		{
			par_shapes_rotate(disk, static_cast<float>(PAR_PI * 0.1f), reinterpret_cast<const float*>(&float3::unitY));

			par_shapes_merge(mesh, disk);
			par_shapes_free_mesh(disk);

			par_shapes_translate(mesh, 0.0f, -height / 2.0f, 0.0f);
			par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5f), reinterpret_cast<const float*>(&float3::unitX));

			cone = new ResourceMesh(numLights, "", "", "");
			++numLights;

			LoadShape(mesh, cone);

			par_shapes_free_mesh(mesh);
		}
	}
	return cone;
}

ResourceMesh* LightProxy::CreateTubeShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cylinder(slices, stacks);
	//par_shapes_mesh* sphere1 = par_shapes_create_parametric_sphere(slices, stacks);
	//par_shapes_mesh* sphere2 = par_shapes_create_parametric_sphere(slices, stacks);
	ResourceMesh* tube = nullptr;

	if (mesh)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5f), reinterpret_cast<const float*>(&float3::unitX));
		par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, radius);

		//par_shapes_scale(sphere1, radius, radius, radius);
		//par_shapes_translate(sphere1, -0.5f, -0.5f + height, 0.0f);
		//
		//par_shapes_scale(sphere2, radius, radius, radius);
		//par_shapes_translate(sphere2, -0.5f, -0.5f - height, 0.0f);

		//mesh = par_shapes_weld(mesh, 0.0001f, nullptr);
		//mesh = par_shapes_weld(sphere1, 0.0001f, nullptr);
		//mesh = par_shapes_weld(sphere2, 0.0000001f, nullptr);

		LoadShape(mesh, tube);

		par_shapes_free_mesh(mesh);
	}
	return tube;
}

void LightProxy::CreatePlaneShape(float height, float radius, unsigned slices, unsigned stacks)
{
	/*par_shapes_mesh* mesh = par_shapes_create_plane(slices, stacks);

	if (mesh)
	{
		par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, 1.0f);

		LoadShape(mesh, plane);

		par_shapes_free_mesh(mesh);
	}*/
}

void LightProxy::ReloadSphereShape(ResourceMesh* sphere, float size, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, stacks);

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);

		LoadShape(mesh, sphere);

		par_shapes_free_mesh(mesh);
	}
}

void LightProxy::ReloadConeShape(ResourceMesh* cone, float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cone(slices, stacks);

	if (mesh)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5f), reinterpret_cast<const float*>(&float3::unitX));
		par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, radius);

		const float3 center = float3(0.0f, -(height / 2.0f), 0.0f);
		const float3 normal = -float3::unitY;
		par_shapes_mesh* disk = par_shapes_create_disk(radius, slices, &center.x, &normal.x);

		if (disk)
		{
			par_shapes_rotate(disk, static_cast<float>(PAR_PI * 0.1f), reinterpret_cast<const float*>(&float3::unitY));

			par_shapes_merge(mesh, disk);
			par_shapes_free_mesh(disk);

			par_shapes_translate(mesh, 0.0f, -height / 2.0f, 0.0f);
			par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5f), reinterpret_cast<const float*>(&float3::unitX));

			LoadShape(mesh, cone);

			par_shapes_free_mesh(mesh);
		}
	}
}

void LightProxy::ReloadTubeShape(ResourceMesh* tube, float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cylinder(slices, stacks);
	//par_shapes_mesh* sphere1 = par_shapes_create_parametric_sphere(slices, stacks);
	//par_shapes_mesh* sphere2 = par_shapes_create_parametric_sphere(slices, stacks);

	if (mesh)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5f), reinterpret_cast<const float*>(&float3::unitX));
		par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, radius);

		//par_shapes_scale(sphere1, radius, radius, radius);
		//par_shapes_translate(sphere1, -0.5f, -0.5f + height, 0.0f);
		//
		//par_shapes_scale(sphere2, radius, radius, radius);
		//par_shapes_translate(sphere2, -0.5f, -0.5f - height, 0.0f);

		//mesh = par_shapes_weld(mesh, 0.0001f, nullptr);
		//mesh = par_shapes_weld(sphere1, 0.0001f, nullptr);
		//mesh = par_shapes_weld(sphere2, 0.0000001f, nullptr);

		LoadShape(mesh, tube);

		par_shapes_free_mesh(mesh);
	}
}
