#include "StdAfx.h"

#include "ComponentLocalIBL.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"

#include "Scene/Scene.h"

#include "ComponentSkybox.h"
#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Program/Program.h"

#include "Geometry/Frustum.h"

#include "debugdraw.h"


#define CUBEMAP_RESOLUTION 512
#define DIFFUSE_RESOLUTION 512
#define SPECULAR_RESOLUTION 512

ComponentLocalIBL::ComponentLocalIBL(GameObject* parent) :
	ComponentLight(LightType::LOCAL_IBL, parent, true), cubemap(0), diffuse(0), preFiltered(0), cubeVAO(0), cubeVBO(0),
	handleIrradiance(0), handlePreFiltered(0), first(true)
{
	if (GetOwner()->HasComponent<ComponentTransform>())
	{
		float3 pos = GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
		parallaxAABB = { pos + float3(-5, -5, -5), pos + float3(5, 5, 5) };
		influenceAABB = { pos + float3(-5, -5, -5), pos + float3(5, 5, 5) };
		originCenterParallax = parallaxAABB.CenterPoint();
		originCenterInfluence = influenceAABB.CenterPoint();
	}
	App->ScheduleTask([this]()
		{
			Initialize();
		});
}

ComponentLocalIBL::ComponentLocalIBL(const ComponentLocalIBL& componentLocalIBL) : ComponentLight(componentLocalIBL), first(true),
parallaxAABB(componentLocalIBL.parallaxAABB), originCenterParallax(componentLocalIBL.originCenterParallax),
influenceAABB(componentLocalIBL.influenceAABB), originCenterInfluence(componentLocalIBL.originCenterInfluence)
{
	Initialize();
	GenerateMaps();
}

ComponentLocalIBL::~ComponentLocalIBL()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &depth);

	glDeleteTextures(1, &cubemap);
	glDeleteTextures(1, &diffuse);
	glDeleteTextures(1, &preFiltered);

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);

	glMakeTextureHandleNonResidentARB(handleIrradiance);
	glMakeTextureHandleNonResidentARB(handlePreFiltered);

	deleting = true;

	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (currentScene)
	{
		currentScene->UpdateSceneLocalIBLs();
		currentScene->RenderLocalIBLs();
	}
}

void ComponentLocalIBL::GenerateMaps()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Local IBL")), "Local IBL");

	// Take screenshots of the scene
	CreateCubemap();

	glCullFace(GL_BACK); // Show back faces	 
	glFrontFace(GL_CCW); // Front faces will be counter clockwise
	glDisable(GL_DEPTH_TEST); // Disable depth testing

	// Render in Irradiance
	Program* irradianceProgram = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::IRRADIANCE_MAP);
	irradianceProgram->Activate();

	irradianceProgram->BindUniformInt("environment", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	
	RenderToCubeMap(diffuse, irradianceProgram, DIFFUSE_RESOLUTION);
	irradianceProgram->Deactivate();

	// Render in pre-filtered
	Program* preFilteredProgram = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PRE_FILTERED_MAP);
	preFilteredProgram->Activate();

	preFilteredProgram->BindUniformInt("environment", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	preFilteredProgram->BindUniformFloat(5, static_cast<float>(CUBEMAP_RESOLUTION));
	for (int mipMap = 0; mipMap <= numMipMaps; ++mipMap)
	{
		float roughness = static_cast<float>(mipMap) / static_cast<float>(numMipMaps);
		preFilteredProgram->BindUniformFloat(4, roughness);

		unsigned int mipResolution = static_cast<unsigned int>(SPECULAR_RESOLUTION * std::pow(0.5, mipMap));
		RenderToCubeMap(preFiltered, preFilteredProgram, mipResolution, mipMap);
	}
	preFilteredProgram->Deactivate();

	glPopDebugGroup();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT); // Show front faces
	glFrontFace(GL_CW); // Clockwise
}

void ComponentLocalIBL::Draw() const
{
	bool canDrawLocalIBL =
#ifdef ENGINE
		IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject();
#else
		false;
#endif // ENGINE

	if (!canDrawLocalIBL)
	{
		return;
	}

	dd::aabb(parallaxAABB.minPoint, parallaxAABB.maxPoint, dd::colors::Red);
	
	dd::aabb(influenceAABB.minPoint, influenceAABB.maxPoint, dd::colors::Green);
}

void ComponentLocalIBL::OnTransformChanged()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	float3 positionParent = GetPosition();

	if (first)
	{
		initialParallaxOffset = originCenterParallax - positionParent;
		initialInfluenceOffset = originCenterInfluence - positionParent;
		first = false;
	}

	float3 parallaxOffset = initialParallaxOffset + positionParent - originCenterParallax;
	float3 influenceOffset = initialInfluenceOffset + positionParent - originCenterInfluence;

	parallaxAABB.Translate(parallaxOffset);
	influenceAABB.Translate(influenceOffset);

	originCenterParallax += parallaxOffset;
	originCenterInfluence += influenceOffset;

	currentScene->UpdateSceneLocalIBL(this);
	currentScene->RenderLocalIBL(this);
}

const uint64_t& ComponentLocalIBL::GetHandleIrradiance()
{
	if (handleIrradiance == 0)
	{
		handleIrradiance = glGetTextureHandleARB(diffuse);
		glMakeTextureHandleResidentARB(handleIrradiance);
	}
	return handleIrradiance;
}

const uint64_t& ComponentLocalIBL::GetHandlePreFiltered()
{
	if (handlePreFiltered == 0)
	{
		handlePreFiltered = glGetTextureHandleARB(preFiltered);
		glMakeTextureHandleResidentARB(handlePreFiltered);
	}
	return handlePreFiltered;
}

const float3& ComponentLocalIBL::GetPosition()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
}

const Quat& ComponentLocalIBL::GetRotation()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalRotation();
}

void ComponentLocalIBL::SetParallaxAABB(AABB& aabb)
{
	parallaxAABB = aabb;
	App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateSceneLocalIBL(this);
	App->GetModule<ModuleScene>()->GetLoadedScene()->RenderLocalIBL(this);
}

void ComponentLocalIBL::SetInfluenceAABB(AABB& aabb)
{
	influenceAABB = aabb;
	App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateSceneLocalIBL(this);
	App->GetModule<ModuleScene>()->GetLoadedScene()->RenderLocalIBL(this);
}

void ComponentLocalIBL::InternalSave(Json& meta)
{
	meta["lightType"] = GetNameByLightType(lightType).c_str();

	meta["parallax_min_x"] = static_cast<float>(parallaxAABB.minPoint.x);
	meta["parallax_min_y"] = static_cast<float>(parallaxAABB.minPoint.y);
	meta["parallax_min_z"] = static_cast<float>(parallaxAABB.minPoint.z);
	
	meta["parallax_max_x"] = static_cast<float>(parallaxAABB.maxPoint.x);
	meta["parallax_max_y"] = static_cast<float>(parallaxAABB.maxPoint.y);
	meta["parallax_max_z"] = static_cast<float>(parallaxAABB.maxPoint.z);

	meta["influence_min_x"] = static_cast<float>(influenceAABB.minPoint.x);
	meta["influence_min_y"] = static_cast<float>(influenceAABB.minPoint.y);
	meta["influence_min_z"] = static_cast<float>(influenceAABB.minPoint.z);

	meta["influence_max_x"] = static_cast<float>(influenceAABB.maxPoint.x);
	meta["influence_max_y"] = static_cast<float>(influenceAABB.maxPoint.y);
	meta["influence_max_z"] = static_cast<float>(influenceAABB.maxPoint.z);	
}

void ComponentLocalIBL::InternalLoad(const Json& meta)
{
	lightType = GetLightTypeByName(meta["lightType"]);

	parallaxAABB.minPoint.x = static_cast<float>(meta["parallax_min_x"]);
	parallaxAABB.minPoint.y = static_cast<float>(meta["parallax_min_y"]);
	parallaxAABB.minPoint.z = static_cast<float>(meta["parallax_min_z"]);

	parallaxAABB.maxPoint.x = static_cast<float>(meta["parallax_max_x"]);
	parallaxAABB.maxPoint.y = static_cast<float>(meta["parallax_max_y"]);
	parallaxAABB.maxPoint.z = static_cast<float>(meta["parallax_max_z"]);

	originCenterParallax = parallaxAABB.CenterPoint();

	influenceAABB.minPoint.x = static_cast<float>(meta["influence_min_x"]);
	influenceAABB.minPoint.y = static_cast<float>(meta["influence_min_y"]);
	influenceAABB.minPoint.z = static_cast<float>(meta["influence_min_z"]);

	influenceAABB.maxPoint.x = static_cast<float>(meta["influence_max_x"]);
	influenceAABB.maxPoint.y = static_cast<float>(meta["influence_max_y"]);
	influenceAABB.maxPoint.z = static_cast<float>(meta["influence_max_z"]);

	originCenterInfluence = influenceAABB.CenterPoint();
}

void ComponentLocalIBL::Initialize()
{
	CreateVAO();

	// Generate framebuffer & renderBuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

	// cubemap
	glGenTextures(1, &cubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			CUBEMAP_RESOLUTION,
			CUBEMAP_RESOLUTION,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// irradianceMap
	glGenTextures(1, &diffuse);

	glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			DIFFUSE_RESOLUTION,
			DIFFUSE_RESOLUTION,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pre-filtered map
	glGenTextures(1, &preFiltered);

	glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			SPECULAR_RESOLUTION,
			SPECULAR_RESOLUTION,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
	}

	numMipMaps = static_cast<int>(log(static_cast<float>(SPECULAR_RESOLUTION)) / log(2));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentLocalIBL::CreateCubemap()
{
	const float3 front[6] = { float3::unitX, -float3::unitX, float3::unitY,
							-float3::unitY, float3::unitZ, -float3::unitZ };
	const float3 up[6] = { -float3::unitY, -float3::unitY, float3::unitZ, 
							-float3::unitZ, -float3::unitY, -float3::unitY };

	ModuleRender* modRender = App->GetModule<ModuleRender>();
	ModuleProgram* modProgram = App->GetModule<ModuleProgram>();
	Scene* scene = App->GetModule<ModuleScene>()->GetLoadedScene();

	ComponentSkybox* skybox = scene->GetRoot()->GetComponentInternal<ComponentSkybox>();

	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
	frustum.SetPos(GetPosition());
	frustum.SetViewPlaneDistances(0.1f, 100.0f);

	glViewport(0, 0, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frustum.SetFront(GetRotation() * front[i]);
		frustum.SetUp(GetRotation() * up[i]);
		modRender->BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFAULT), frustum);
		modRender->BindCameraToProgram(modProgram->GetProgram(ProgramType::SPECULAR), frustum);

		if (skybox)
		{
			skybox->Draw(frustum.ViewMatrix(), frustum.ProjectionMatrix());
		}

		std::vector<GameObject*> objectsInFrustum = scene->ObtainStaticObjectsInFrustum(&frustum);

		modRender->SortOpaques(objectsInFrustum, frustum.Pos());
		modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::DEFAULT, false);
		modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::SPECULAR, false);
		modRender->SortTransparents(objectsInFrustum, frustum.Pos());
		modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::DEFAULT);
		modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::SPECULAR);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void ComponentLocalIBL::RenderToCubeMap(unsigned int cubemapTex, Program* usedProgram, int resolution, int mipmapLevel)
{
	const float3 front[6] = { float3::unitX, -float3::unitX, float3::unitY,
							-float3::unitY, float3::unitZ, -float3::unitZ };
	const float3 up[6] = { -float3::unitY, -float3::unitY, float3::unitZ,
							-float3::unitZ, -float3::unitY, -float3::unitY };
	
	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
	frustum.SetPos(float3::zero);
	frustum.SetViewPlaneDistances(0.1f, 100.0f);

	usedProgram->BindUniformFloat4x4(1, frustum.ProjectionMatrix().ptr(), GL_TRUE);

	glViewport(0, 0, resolution, resolution);
	glBindVertexArray(cubeVAO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTex, mipmapLevel);

		frustum.SetFront(front[i]);
		frustum.SetUp(up[i]);

		usedProgram->BindUniformFloat4x4(0, frustum.ViewMatrix().ptr(), GL_TRUE);
		// Draw cube
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void ComponentLocalIBL::CreateVAO()
{
	float vertices[108] = { // Front (x, y, z)
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,

							// Left (x, y, z)
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,

							// Right (x, y, z)
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,

							// Back (x, y, z)
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,

							// Top (x, y, z)
							-1.0f,
							1.0f,
							-1.0f,
							1.0f,
							1.0f,
							-1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							1.0f,
							-1.0f,
							1.0f,
							1.0f,
							-1.0f,
							1.0f,
							-1.0f,

							// Bottom (x, y, z)
							-1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							-1.0f,
							1.0f,
							1.0f,
							-1.0f,
							1.0f
	};
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}