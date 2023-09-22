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
	cylinder = new ResourceMesh(3, "", "", "");
	plane = new ResourceMesh(4, "", "", "");
}

LightProxy::~LightProxy()
{
	delete sphere;
	delete cone;
	delete cylinder;
}

void LightProxy::DrawLights(Program* program)
{
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Light Culling"), "Light Culling");

	// Draw point lights
	std::vector<std::pair<const ComponentPointLight*, unsigned int>> points = 
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedPointLights();

	for (std::pair<const ComponentPointLight*, unsigned int> point : points)
	{
		float radius = point.first->GetRadius();
		float4x4 transform = point.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		SphereShape(radius, 15, 15);

		program->BindUniformFloat4x4("model", &transform[0][0], true);

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

	// Draw spot lights
	std::vector<std::pair<const ComponentSpotLight*, unsigned int>> spots =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedSpotLights();

	for (std::pair<const ComponentSpotLight*, unsigned int> spot : spots)
	{
		float height = spot.first->GetRadius();
		float radius = height * math::Tan(spot.first->GetOuterAngle());

		ConeShape(height, radius, 15, 15);

		float4x4 transform = spot.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		program->BindUniformFloat4x4("model", &transform[0][0], true);

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

	// Draw Sphere Lights
	std::vector<std::pair<const ComponentAreaLight*, unsigned int>> spheres =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetCachedSphereLights();

	for (std::pair<const ComponentAreaLight*, unsigned int> sphere : spheres)
	{
		float radius = sphere.first->GetAttRadius() + sphere.first->GetShapeRadius();
		float4x4 transform = sphere.first->GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalMatrix();

		SphereShape(radius, 15, 15);

		program->BindUniformFloat4x4("model", &transform[0][0], true);

		glBindVertexArray(this->sphere->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->sphere->GetEBO());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, this->sphere->GetNumIndexes(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}

	glPopDebugGroup();

	program->Deactivate();
}

void LightProxy::LoadShape(par_shapes_mesh* shape, ResourceMesh* mesh)
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

void LightProxy::CylinderShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cylinder(slices, stacks);

	if (mesh)
	{
		par_shapes_rotate(mesh, -static_cast<float>(PAR_PI * 0.5), reinterpret_cast<const float*>(&float3::unitX));
		par_shapes_translate(mesh, 0.0f, -0.5f, 0.0f);
		par_shapes_scale(mesh, radius, height, radius);

		LoadShape(mesh, cylinder);

		par_shapes_free_mesh(mesh);
	}
}

void LightProxy::PlaneShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_plane(slices, stacks);
	par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);

	if (mesh)
	{
		par_shapes_scale(mesh, radius, height, 1.0f);

		LoadShape(mesh, plane);

		par_shapes_free_mesh(mesh);
	}
}
