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


LightProxy::LightProxy(): numPointLight(0),numSpotLight(0)
{
	sphere = new ResourceMesh(1,"","","");
	cone = new ResourceMesh(2, "", "", "");
	tube = new ResourceMesh(3, "", "", "");
	plane = new ResourceMesh(4, "", "", "");
}

LightProxy::~LightProxy()
{
	delete sphere;
	delete cone;
	delete tube;
}

void LightProxy::DrawLights(Program* program)
{
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Light Culling"), "Light Culling");

	program->BindUniformFloat2("screenSize", float2(screenSize.first, screenSize.second));

	DrawPoints(program);
	DrawSpots(program);
	DrawSpheres(program);
	//DrawTubes(program); //No worth to draw, needed too many welds to get a tube mesh

	glPopDebugGroup();

	program->Deactivate();
}

void LightProxy::DrawPoints(Program* program)
{
	std::vector<std::pair<const ComponentPointLight*, unsigned int>> points =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedPointLights();

	for (std::pair<const ComponentPointLight*, unsigned int> point : points)
	{
		float radius = point.first->GetRadius();
		float4x4 transform = point.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		SphereShape(radius, 15, 15);

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		program->BindUniformInt("lightIndex", static_cast<int>(point.second));
		program->BindUniformInt("flagPoint", 1);
		program->BindUniformInt("flagSpot", 0);
		program->BindUniformInt("flagSphere", 0);
		program->BindUniformInt("flagTube", 0);

		glBindVertexArray(sphere->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->GetEBO());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, sphere->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}

void LightProxy::DrawSpots(Program* program)
{
	std::vector<std::pair<const ComponentSpotLight*, unsigned int>> spots =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedSpotLights();

	for (std::pair<const ComponentSpotLight*, unsigned int> spot : spots)
	{
		float height = spot.first->GetRadius();
		float radius = height * math::Tan(spot.first->GetOuterAngle());

		ConeShape(height, radius, 15, 15);

		float4x4 transform = spot.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		program->BindUniformInt("lightIndex", static_cast<int>(spot.second));
		program->BindUniformInt("flagSpot", 1);
		program->BindUniformInt("flagPoint", 0);
		program->BindUniformInt("flagSphere", 0);
		program->BindUniformInt("flagTube", 0);

		glBindVertexArray(cone->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cone->GetEBO());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, cone->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}

void LightProxy::DrawSpheres(Program* program)
{
	std::vector<std::pair<const ComponentAreaLight*, unsigned int>> spheres =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedSphereLights();

	for (std::pair<const ComponentAreaLight*, unsigned int> sphere : spheres)
	{
		float radius = sphere.first->GetAttRadius() + sphere.first->GetShapeRadius();
		float4x4 transform = sphere.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		SphereShape(radius, 15, 15);

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		program->BindUniformInt("lightIndex", static_cast<int>(sphere.second));
		program->BindUniformInt("flagSphere", 1);
		program->BindUniformInt("flagSpot", 0);
		program->BindUniformInt("flagPoint", 0);
		program->BindUniformInt("flagTube", 0);

		glBindVertexArray(this->sphere->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->sphere->GetEBO());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);

		glDrawElements(GL_TRIANGLES, this->sphere->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glFrontFace(GL_CW);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}

void LightProxy::DrawTubes(Program* program)
{
	std::vector<std::pair<const ComponentAreaLight*, unsigned int>> tubes =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedTubeLights();

	for (std::pair<const ComponentAreaLight*, unsigned int> tube : tubes)
	{
		float height = tube.first->GetHeight();
		float radius = tube.first->GetAttRadius() + tube.first->GetShapeRadius();

		TubeShape(height, radius, 15, 5);

		float4x4 transform = tube.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);
		
		program->BindUniformInt("lightIndex", static_cast<int>(tube.second));
		program->BindUniformInt("flagTube", 1);
		program->BindUniformInt("flagSphere", 0);
		program->BindUniformInt("flagSpot", 0);
		program->BindUniformInt("flagPoint", 0);

		glBindVertexArray(this->tube->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->tube->GetEBO());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);

		glDrawElements(GL_TRIANGLES, this->tube->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glFrontFace(GL_CW);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
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

void LightProxy::SphereShape(float size, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, stacks);

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);

		LoadShape(mesh, sphere);

		par_shapes_free_mesh(mesh);
	}
}

void LightProxy::ConeShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cone(slices, stacks);

	if (mesh)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5), reinterpret_cast<const float*>(&float3::unitX));
		par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, radius);
		par_shapes_translate(mesh, 0.0f, -height/2.0f, 0.0f);
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5), reinterpret_cast<const float*>(&float3::unitX));

		LoadShape(mesh, cone);

		par_shapes_free_mesh(mesh);
	}

}

void LightProxy::TubeShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cylinder(slices, stacks);
	//par_shapes_mesh* sphere1 = par_shapes_create_parametric_sphere(slices, stacks);
	//par_shapes_mesh* sphere2 = par_shapes_create_parametric_sphere(slices, stacks);

	if (tube && sphere)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5), reinterpret_cast<const float*>(&float3::unitX));
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

void LightProxy::PlaneShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_plane(slices, stacks);

	if (mesh)
	{
		par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, 1.0f);

		LoadShape(mesh, plane);

		par_shapes_free_mesh(mesh);
	}
}
