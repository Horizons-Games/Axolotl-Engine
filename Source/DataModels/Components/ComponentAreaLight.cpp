#include "ComponentAreaLight.h"

#include "ComponentMeshRenderer.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"

#include "Math/float3.h"
#include <vector>
#include <memory>


#define PAR_SHAPES_IMPLEMENTATION
#include "Auxiliar/Test/par_shapes.h"

#define SECTOR_SPHERE_COUNT 36
#define STACK_SPHERE_COUNT 34
#define SECTOR_TUBE_COUNT 20
#define STACK_TUBE_COUNT 4

ComponentAreaLight::ComponentAreaLight() : ComponentLight(LightType::AREA, true)
{
}

ComponentAreaLight::ComponentAreaLight(const ComponentAreaLight& componentAreaLight) :
	ComponentLight(componentAreaLight), areaType(componentAreaLight.areaType), shapeRadius(componentAreaLight.shapeRadius),
	lightRadius(componentAreaLight.lightRadius)
{

}

ComponentAreaLight::ComponentAreaLight(AreaType areaType, GameObject* parent) :
	ComponentLight(LightType::AREA, parent, true), areaType(areaType), shapeRadius(1.f), lightRadius(2.f)
{	
	par_shapes_mesh* s;
	ComponentMeshRenderer* testComp = static_cast<ComponentMeshRenderer*>(owner->CreateComponent(ComponentType::MESHRENDERER));
	std::shared_ptr<ResourceMesh> test = std::make_shared<ResourceMesh>();
	std::shared_ptr<ResourceMaterial> material = std::make_shared<ResourceMaterial>(0, "", "", "");
	material.get()->SetDiffuseColor(float4(1, 1, 1, 1)); 
	material.get()->SetShaderType(0);
	
	switch (areaType)
	{
	case AreaType::SPHERE:
		s = par_shapes_create_parametric_sphere(SECTOR_SPHERE_COUNT, STACK_SPHERE_COUNT);
		
		break;
	case AreaType::TUBE:
		s = par_shapes_create_cylinder(SECTOR_TUBE_COUNT, STACK_TUBE_COUNT);

		break;
	case AreaType::QUAD:
		s = par_shapes_create_parametric_sphere(SECTOR_SPHERE_COUNT, STACK_SPHERE_COUNT);

		break;
	case AreaType::DISC:
		s = par_shapes_create_parametric_sphere(SECTOR_SPHERE_COUNT, STACK_SPHERE_COUNT);

		break;
	case AreaType::NONE:
		s = par_shapes_create_parametric_sphere(SECTOR_SPHERE_COUNT, STACK_SPHERE_COUNT);

		break;
	default:
		s = par_shapes_create_parametric_sphere(SECTOR_SPHERE_COUNT, STACK_SPHERE_COUNT);

		break;
	}
	std::vector<float3> vertices, normals, uvs;
	std::vector<std::vector<unsigned int>> triangles;
	float const* points = s->points;
	float const* tcoords = s->tcoords;
	float const* norms = s->normals;
	PAR_SHAPES_T const* indices = s->triangles;

	for (int nvert = 0; nvert < s->npoints; nvert++) {
		float3 vertice = float3(points[0], points[1], points[2]);
		if (tcoords)
		{
			float3 uv = float3(tcoords[0], tcoords[1], 0);
			uvs.push_back(uv);
			tcoords += 2;
		}
		if (norms)
		{
			float3 normal = float3(norms[0], norms[1], norms[2]);
			normals.push_back(normal);
			norms += 3;
		}
		vertices.push_back(vertice);
		points += 3;
	}
	for (int nface = 0; nface < s->ntriangles; nface++) {
		unsigned int a = *indices++;
		unsigned int b = *indices++;
		unsigned int c = *indices++;
		std::vector<unsigned int> aux{ a, b, c };
		triangles.push_back(aux);
	}
	test->SetVertices(vertices);
	test->SetFacesIndices(triangles);
	if (tcoords)
	{
		test->SetTextureCoords(uvs);
	}
	if (norms)
	{
		test->SetNormals(normals);
	}
	test->SetNumVertices(vertices.size());
	test->SetNumFaces(triangles.size());
	test->SetAttach(std::vector<Attach>(s->npoints));
	testComp->SetMesh(test);
	testComp->SetMaterial(material);
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, true), areaType(areaType), shapeRadius(1.f), lightRadius(2.f)
{
	std::vector<float3> vertices, normals, uvs;
	std::vector<std::vector<unsigned int>> triangles;
	par_shapes_mesh* s = par_shapes_create_parametric_sphere(SECTOR_SPHERE_COUNT, STACK_SPHERE_COUNT);
	ComponentMeshRenderer* testComp = static_cast<ComponentMeshRenderer*>(owner->CreateComponent(ComponentType::MESHRENDERER));
	std::shared_ptr<ResourceMesh> test = std::make_shared<ResourceMesh>();
	std::shared_ptr<ResourceMaterial> material = std::make_shared<ResourceMaterial>(0, "", "", "");
	material.get()->SetDiffuseColor(float4(1, 1, 1, 1));
	material.get()->SetShaderType(0);
	float const* points = s->points;
	float const* tcoords = s->tcoords;
	float const* norms = s->normals;
	PAR_SHAPES_T const* indices = s->triangles;
	switch (areaType)
	{
	case AreaType::SPHERE:
		test->SetNumFaces(s->ntriangles);
		test->SetNumVertices(s->npoints);
		for (int nvert = 0; nvert < s->npoints; nvert++) {
			float3 vertice = float3(points[0], points[1], points[2]);
			if (tcoords)
			{
				float3 uv = float3(tcoords[0], tcoords[1], 0);
				uvs.push_back(uv);
				tcoords += 2;
			}
			if (norms)
			{
				float3 normal = float3(norms[0], norms[1], norms[2]);
				normals.push_back(normal);
				norms += 3;
			}
			vertices.push_back(vertice);
			points += 3;
		}
		for (int nface = 0; nface < s->ntriangles; nface++) {
			unsigned int a = 1 + *indices++;
			unsigned int b = 1 + *indices++;
			unsigned int c = 1 + *indices++;
			std::vector<unsigned int> aux{ a, b, c };
			triangles.push_back(aux);
		}
		test->SetVertices(vertices);
		test->SetFacesIndices(triangles);
		if (tcoords)
		{
			test->SetTextureCoords(uvs);
		}
		if (norms)
		{
			test->SetNormals(normals);
		}
		test->SetAttach(std::vector<Attach>(s->npoints));
		testComp->SetMesh(test);
		testComp->SetMaterial(material);
		break;
	case AreaType::TUBE:
		break;
	case AreaType::QUAD:
		break;
	case AreaType::DISC:
		break;
	case AreaType::NONE:
		break;
	default:
		break;
	}
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, parent, true), areaType(areaType), shapeRadius(1.f)
{
}

ComponentAreaLight::~ComponentAreaLight()
{
}
