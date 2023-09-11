#include "StdAfx.h"
#include "LightProxy.h"
#include "Application.h"
#include "par_shapes.h"


#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"

#include "Camera/Camera.h"
#include "Scene/Scene.h"
#include "Program/Program.h"


LightProxy::LightProxy()
{
}

LightProxy::~LightProxy()
{
}

void LightProxy::DrawAreaLights(Program* program, GLuint frameBuffer)
{
	numPointLight = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSizePointLights();
	numSpotLight = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSizeSpotLights();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	program->Activate();

	for (auto i = 0; i < numPointLight; i++)
	{
	}

	glBindVertexArray(sphere->GetVAO());
	glDrawElementsInstanced(GL_TRIANGLES, sphere->GetNumVertices(), GL_UNSIGNED_INT, nullptr, sphere->GetNumIndexes());
	glBindVertexArray(0);

	for (auto i = 0; i < numSpotLight; i++)
	{
	}

	glBindVertexArray(cone->GetVAO());
	glDrawElementsInstanced(GL_TRIANGLES, sphere->GetNumVertices(), GL_UNSIGNED_INT, nullptr, cone->GetNumIndexes());

	glBindVertexArray(0);

	glDisable(GL_BLEND);
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

		std::vector<float3> normals(mesh->normals, mesh->normals + (mesh->npoints * 3));
		sphere->SetNormals(normals);

		std::vector<float2> tcoordsCopy(mesh->tcoords, mesh->tcoords + (mesh->npoints * 2));
		std::vector<float3> tcoords;
		for (int i = 0; i < tcoordsCopy.size(); i++)
		{
			tcoords.push_back(float3(tcoordsCopy[i].x, tcoordsCopy[i].y, 0.0f));
		}
		sphere->SetTextureCoords(tcoords);

		sphere->SetNumFaces(mesh->ntriangles);
		std::vector<std::vector<unsigned int>> facesIndexes(mesh->triangles, mesh->triangles + (mesh->ntriangles * 3));

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
		cone->SetNumIndexes(mesh->ntriangles);
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
