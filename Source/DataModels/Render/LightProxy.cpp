#include "StdAfx.h"
#include "LightProxy.h"
#include "Application.h"
#include "par_shapes.h"
#include "Math/TransformOps.h"
#include "Components/ComponentTransform.h"


#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"

#include "Camera/Camera.h"
#include "Scene/Scene.h"
#include "Program/Program.h"

LightProxy::LightProxy(): numPointLight(0),numSpotLight(0)
{
	sphere = new ResourceMesh(0,"","","");
	cone = new ResourceMesh(1, "", "", "");
	cylinder = new ResourceMesh(2, "", "", "");
}

LightProxy::~LightProxy()
{
	delete sphere;
	delete cone;
	delete cylinder;
}

void LightProxy::DrawAreaLights(Program* program, GLuint frameBuffer)
{
	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
	if (loadedScene->GetSizePointLights() > 0)
	{
		numPointLight = loadedScene->GetSizePointLights();
	}
	if (loadedScene->GetSizeSpotLights() > 0)
	{
		numSpotLight = loadedScene->GetSizeSpotLights();
	}

	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();
	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	program->Activate();
	program->BindUniformFloat4x4("proj", proj.ptr(), true);
	program->BindUniformFloat4x4("view", view.ptr(), true);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	int num = 0;
	for (int i = 0, num = numPointLight; i < num; ++i)
	{
		const ComponentPointLight* pointlight = loadedScene->GetPointLight(i);
		if (pointlight->GetOwner()->IsEnabled())
		{
			//It's a sphere so don't need rotation and a scale from radius is enough
			float4x4 model = static_cast<float4x4>(float4x4::UniformScale(pointlight->GetRadius()));

			SphereShape(pointlight->GetRadius(),20,20);
		}
		glBindVertexArray(sphere->GetVAO());
		glDrawElementsInstanced(GL_TRIANGLES, sphere->GetNumVertices(), GL_UNSIGNED_INT, nullptr, sphere->GetNumIndexes());
		glBindVertexArray(0);
	}

	for (int i = 0, num = numSpotLight; i < num; ++i)
	{
		const ComponentSpotLight* spotlight = loadedScene->GetSpotLight(i);
		SpotLight light = loadedScene->GetSpotLightsStruct(i);
		if (spotlight->GetOwner()->IsEnabled())
		{
			const ComponentTransform* transform = spotlight->GetOwner()->GetComponentInternal<ComponentTransform>();

			float3 position = transform->GetGlobalPosition();
			float4x4 model = float4x4::identity;
			model = float4x4::Translate(position);
			model = float4x4::RotateFromTo(transform->GetRotationXYZ(),light.aim);

			// Apply scaling to create the cone shape
			//GetHeight ???
			model = model * float4x4::Scale(spotlight->GetRadius(), spotlight->GetInnerAngle(),
				spotlight->GetOuterAngle());


			ConeShape(spotlight->GetRadius(),spotlight->GetRadius(),20,20);
		}
		glBindVertexArray(cone->GetVAO());
		glDrawElementsInstanced(GL_TRIANGLES, cone->GetNumVertices(), GL_UNSIGNED_INT, nullptr, cone->GetNumIndexes());
		glBindVertexArray(0);
	}


	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	program->Deactivate();
}

void LightProxy::SphereShape(float size, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_parametric_sphere(slices, stacks);

	if (mesh)
	{
		par_shapes_scale(mesh, size, size, size);

		sphere->SetNumVertices(mesh->npoints);
		std::vector<float3> vertices(mesh->points, mesh->points + (mesh->npoints * 3));
		sphere->SetVertices(vertices);
		cone->SetNumIndexes(mesh->ntriangles);

		std::vector<float3> normals(mesh->normals, mesh->normals + (mesh->npoints * 3));
		sphere->SetNormals(normals);

		std::vector<float2> tcoordsCopy(mesh->tcoords, mesh->tcoords + (mesh->npoints * 2));
		std::vector<float3> tcoords;
		for (int i = 0; i < tcoordsCopy.size(); i++)
		{
			tcoords.push_back(float3(tcoordsCopy[i].x, tcoordsCopy[i].y, 0.0f));
		}
		sphere->SetTextureCoords(tcoords);

		std::vector<unsigned int> facesIndexes(mesh->triangles, mesh->triangles + (mesh->ntriangles * 3));
		std::vector<std::vector<unsigned int>> faces;
		sphere->SetNumFaces(mesh->ntriangles);
		for (unsigned int i = 0; i + 2 < (sphere->GetNumFaces() * 3); i += 3)
		{
			std::vector<unsigned int> indexes{ facesIndexes[i], facesIndexes[i + 1], facesIndexes[i + 2] };
			faces.push_back(indexes);
		}
		sphere->SetFacesIndices(faces);

		sphere->Load();

		par_shapes_free_mesh(mesh);
	}
}

void LightProxy::ConeShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cone(slices, stacks);

	if (mesh)
	{
		par_shapes_scale(mesh, radius, height, radius);

		cone->SetNumVertices(mesh->npoints);
		std::vector<float3> vertices(mesh->points, mesh->points + (mesh->npoints * 3));
		sphere->SetVertices(vertices);
		cone->SetNumIndexes(mesh->ntriangles);

		std::vector<float3> normals(mesh->normals, mesh->normals + (mesh->npoints * 3));
		cone->SetNormals(normals);

		std::vector<float2> tcoordsCopy(mesh->tcoords, mesh->tcoords + (mesh->npoints * 2));
		std::vector<float3> tcoords;
		for (int i = 0; i < tcoordsCopy.size(); i++)
		{
			tcoords.push_back(float3(tcoordsCopy[i].x, tcoordsCopy[i].y, 0.0f));
		}
		cone->SetTextureCoords(tcoords);

		std::vector<unsigned int> facesIndexes(mesh->triangles, mesh->triangles + (mesh->ntriangles * 3));
		std::vector<std::vector<unsigned int>> faces;
		cone->SetNumFaces(mesh->ntriangles);
		for (unsigned int i = 0; i + 2 < (cone->GetNumFaces() * 3); i += 3)
		{
			std::vector<unsigned int> indexes{ facesIndexes[i], facesIndexes[i + 1], facesIndexes[i + 2] };
			faces.push_back(indexes);
		}
		cone->SetFacesIndices(faces);

		cone->Load();
		par_shapes_free_mesh(mesh);
	}

}

void LightProxy::CylinderShape(float height, float radius, unsigned slices, unsigned stacks)
{
	par_shapes_mesh* mesh = par_shapes_create_cylinder(slices, stacks);

	if (mesh)
	{
		par_shapes_scale(mesh, radius, height, radius);

		cone->SetNumVertices(mesh->npoints);
		cone->SetNumIndexes(mesh->ntriangles);
		par_shapes_free_mesh(mesh);
	}
}
