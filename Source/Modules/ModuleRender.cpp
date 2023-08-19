#include "StdAfx.h"

#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModulePlayer.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "Camera/CameraGameObject.h"

#include "Cubemap/Cubemap.h"

#include "Components/ComponentDirLight.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentLine.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Batch/BatchManager.h"
#include "DataModels/Skybox/Skybox.h"
#include "DataModels/GBuffer/GBuffer.h"

#include "debugdraw.h"

#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"

#include "DataStructures/Quadtree.h"

#include "Program/Program.h"

#include "Scene/Scene.h"
#include "Camera/Camera.h"

#include "Skybox/Skybox.h"

#ifdef DEBUG
	#include "optick.h"
#endif // DEBUG

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
}

void __stdcall OurOpenGLErrorFunction(GLenum source,
									  GLenum type,
									  GLuint id,
									  GLenum severity,
									  GLsizei length,
									  const GLchar* message,
									  const void* userParam)
{
	const char *tmpSource = "", *tmpType = "", *tmpSeverity = "";

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:
			tmpSource = "API";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			tmpSource = "Window System";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			tmpSource = "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			tmpSource = "Third Party";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			tmpSource = "Application";
			break;
		case GL_DEBUG_SOURCE_OTHER:
			tmpSource = "Other";
			break;
	};

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:
			tmpType = "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			tmpType = "Deprecated Behaviour";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			tmpType = "Undefined Behaviour";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			tmpType = "Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			tmpType = "Performance";
			break;
		case GL_DEBUG_TYPE_MARKER:
			tmpType = "Marker";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			tmpType = "Push Group";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			tmpType = "Pop Group";
			break;
		case GL_DEBUG_TYPE_OTHER:
			tmpType = "Other";
			break;
	};

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
			tmpSeverity = "high";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			tmpSeverity = "medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			tmpSeverity = "low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			tmpSeverity = "notification";
			break;
	};
}

ModuleRender::ModuleRender() :
	context(nullptr),
	depthStencilRenderBuffer(0),
	bloomActivation(1),
	toneMappingMode(2)
{
}

ModuleRender::~ModuleRender()
{
	delete batchManager;
	delete gBuffer;
	objectsInFrustrumDistances.clear();
	gameObjectsInFrustrum.clear();

	glDeleteFramebuffers(2, &frameBuffer[0]);
	glDeleteTextures(2, &renderedTexture[0]);

	glDeleteRenderbuffers(1, &depthStencilRenderBuffer);

	glDeleteFramebuffers(1, &shadowMapBuffer);
	glDeleteTextures(1, &gShadowMap);
}

bool ModuleRender::Init()
{
	ModuleWindow* window = App->GetModule<ModuleWindow>();
	LOG_VERBOSE("--------- Render Init ----------");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	 // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	context = SDL_GL_CreateContext(window->GetWindow());

	backgroundColor = float4(0.f, 0.f, 0.f, 1.f);

	batchManager = new BatchManager();
	gBuffer = new GBuffer();
	renderShadows = true;

	GLenum err = glewInit();
	// check for errors
	LOG_INFO("glew error {}", glewGetErrorString(err));
	// Should be 2.0
	LOG_INFO("Using Glew {}", glewGetString(GLEW_VERSION));

	LOG_INFO("Vendor: {}", glGetString(GL_VENDOR));
	LOG_INFO("Renderer: {}", glGetString(GL_RENDERER));
	LOG_INFO("OpenGL version supported {}", glGetString(GL_VERSION));
	LOG_INFO("GLSL: {}\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glEnable(GL_CULL_FACE); // Enable face culling
	glFrontFace(GL_CW);	 // Front faces will be clockwise
	glCullFace(GL_FRONT);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenFramebuffers(1, &frameBuffer[0]);
	glGenTextures(1, &renderedTexture[0]);
#ifdef ENGINE
	glGenFramebuffers(1, &frameBuffer[1]);
	glGenTextures(1, &renderedTexture[1]);
#endif // ENGINE

	glGenFramebuffers(BLOOM_BLUR_PING_PONG, bloomBlurFramebuffers);
	glGenTextures(BLOOM_BLUR_PING_PONG, bloomBlurTextures);
	glGenRenderbuffers(1, &depthStencilRenderBuffer);

	glGenFramebuffers(1, &shadowMapBuffer);
	glGenTextures(1, &gShadowMap);

	glGenTextures(1, &parallelReductionInTexture);
	glGenTextures(1, &parallelReductionOutTexture);

	glGenBuffers(1, &minMaxBuffer);

	std::pair<int, int> windowSize = window->GetWindowSize();
	UpdateBuffers(windowSize.first, windowSize.second);

	//Reserve space for Camera matrix
	glGenBuffers(1, &uboCamera);
	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);

	const unsigned bindingCamera = 0;

	glBindBufferRange(GL_UNIFORM_BUFFER, bindingCamera, uboCamera, 0, sizeof(float4) * 8);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return true;
}

UpdateStatus ModuleRender::PreUpdate()
{
	int width, height;

	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);
	glViewport(0, 0, width, height);

	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	
	gameObjectsInFrustrum.clear();
	objectsInFrustrumDistances.clear();

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateRender", Optick::Category::Rendering);
#endif // DEBUG

	ModuleWindow* window = App->GetModule<ModuleWindow>();
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	ModuleDebugDraw* debug = App->GetModule<ModuleDebugDraw>();
	ModuleScene* scene = App->GetModule<ModuleScene>();
	ModulePlayer* modulePlayer = App->GetModule<ModulePlayer>();
	const ModuleProgram* modProgram = App->GetModule<ModuleProgram>();

	Scene* loadedScene = scene->GetLoadedScene();

	const Skybox* skybox = loadedScene->GetSkybox();

	GameObject* player = modulePlayer->GetPlayer(); // we can make all of this variables a class variable to save time

#ifdef ENGINE
	if (App->IsOnPlayMode())
#else
	if (player)
#endif
	{
		AddToRenderList(player);
	}

	GameObject* goSelected = App->GetModule<ModuleScene>()->GetSelectedGameObject();

	bool isRoot = goSelected->GetParent() == nullptr;

	FillRenderList(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree());
	
	std::vector<GameObject*> nonStaticsGOs = App->GetModule<ModuleScene>()->GetLoadedScene()->GetNonStaticObjects();

	for (GameObject* nonStaticObj : nonStaticsGOs)
	{
		AddToRenderList(nonStaticObj);
	}
	AddToRenderList(goSelected);

	if (App->GetModule<ModuleDebugDraw>()->IsShowingBoundingBoxes())
	{
		DrawQuadtree(loadedScene->GetRootQuadtree());
	}

	int w, h;
	SDL_GetWindowSize(window->GetWindow(), &w, &h);

	// Bind camera and cubemap info to the shaders
	BindCubemapToProgram(modProgram->GetProgram(ProgramType::DEFAULT));
	BindCubemapToProgram(modProgram->GetProgram(ProgramType::SPECULAR));
	BindCubemapToProgram(modProgram->GetProgram(ProgramType::DEFERRED_LIGHT));
	BindCameraToProgram(modProgram->GetProgram(ProgramType::G_METALLIC));
	BindCameraToProgram(modProgram->GetProgram(ProgramType::G_SPECULAR));

	// -------- DEFERRED GEOMETRY -----------
	gBuffer->BindFrameBuffer();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00); // disable writing to the stencil buffer 

	// Draw opaque objects
	batchManager->DrawOpaque(false);
	if (!App->IsOnPlayMode() && !isRoot)
	{
		// Draw selected opaque
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		batchManager->DrawOpaque(true);

		glDisable(GL_STENCIL_TEST);
	}

	// -------- SHADOW MAP --------
	if (renderShadows)
	{
		// ---- PARALLEL REDUCTION ----
		Program* shadowProgram = modProgram->GetProgram(ProgramType::PARALLEL_REDUCTION);
		float2 minMax = ParallelReduction(shadowProgram, w, h);

		RenderShadowMap(loadedScene->GetDirectionalLight(), minMax);
	}

	BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFAULT));
	BindCameraToProgram(modProgram->GetProgram(ProgramType::SPECULAR));
	BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFERRED_LIGHT));

	// -------- DEFERRED LIGHTING ---------------

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[0]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Program* program = modProgram->GetProgram(ProgramType::DEFERRED_LIGHT);
	program->Activate();

	gBuffer->BindTexture();

	// Binding Shadow map depth buffer
	if (renderShadows)
	{
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, gShadowMap);

		float4x4 lightSpaceMatrix = dirLightProj * dirLightView;
		ComponentDirLight* directLight =
			static_cast<ComponentDirLight*>
			(App->GetModule<ModuleScene>()->GetLoadedScene()->GetDirectionalLight()->GetComponent<ComponentLight>());

		program->BindUniformFloat4x4("lightSpaceMatrix", reinterpret_cast<const float*>(&lightSpaceMatrix), GL_TRUE);
		program->BindUniformFloat("minBias", directLight->shadowBias[0]);
		program->BindUniformFloat("maxBias", directLight->shadowBias[1]);
	}
	program->BindUniformInt("useShadows", static_cast<int>(renderShadows));

	//Use to debug other Gbuffer/value default = 0 position = 1 normal = 2 diffuse = 3 specular = 4 and emissive = 5
	program->BindUniformInt("renderMode", modeRender);

	glDrawArrays(GL_TRIANGLES, 0, 3); // render Quad

	program->Deactivate();

	int width, height;

	SDL_GetWindowSize(window->GetWindow(), &width, &height);

	gBuffer->ReadFrameBuffer();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer[0]);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer[0]);

	// -------- PRE-FORWARD ----------------------

	if (skybox)
	{
		skybox->Draw();
	}

	debug->Draw(camera->GetCamera()->GetViewMatrix(), camera->GetCamera()->GetProjectionMatrix(), width, height);

	// -------- DEFERRED + FORWARD ---------------

	// Draw Transparent objects
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	batchManager->DrawTransparent(false);

	if (!App->IsOnPlayMode() && !isRoot)
	{
		// Draw selected transparent
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		batchManager->DrawTransparent(true);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //discard the ones that are previously captured
		glLineWidth(25);
		glPolygonMode(GL_BACK, GL_LINE);

		// Draw Highliht for selected objects
		DrawHighlight(goSelected);

		glPolygonMode(GL_FRONT, GL_FILL);
		glLineWidth(1);
		glStencilMask(0x00); // disable writing to the stencil buffer 
		glDisable(GL_STENCIL_TEST);
	}
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_BACK, GL_FILL);

	// Draw Particles
	for (ComponentParticleSystem* particle : loadedScene->GetSceneParticleSystems())
	{
		particle->Render();
	}

	glEnable(GL_CULL_FACE); // Enable face culling
	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT, GL_FILL);

	glDisable(GL_BLEND);

	//ComponentLine
	glDisable(GL_CULL_FACE);

	//additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (ComponentLine* lines : loadedScene->GetSceneComponentLines())
	{
		lines->Render();
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDisable(GL_BLEND);

	for (const GameObject* go : gameObjectsInFrustrum)
	{
		go->Render();
	}

	// -------- POST EFFECTS ---------------------

	KawaseDualFiltering();

	// Color correction
#ifdef ENGINE
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[1]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#else
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // default_frame_buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
#endif // ENGINE

	Program* colorCorrectionProgram = modProgram->GetProgram(ProgramType::COLOR_CORRECTION);
	colorCorrectionProgram->Activate();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bloomBlurTextures[0]);

	colorCorrectionProgram->BindUniformInt("tonneMappingMode", toneMappingMode);
	colorCorrectionProgram->BindUniformInt("bloomActivation", bloomActivation);

	glDrawArrays(GL_TRIANGLES, 0, 3); // render Quad
	colorCorrectionProgram->Deactivate();

	// ---- DRAW ALL COMPONENTS IN THE FRUSTRUM --

	for (const GameObject* go : gameObjectsInFrustrum)
	{
		go->Draw();
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

#ifndef ENGINE
	if (!App->IsDebuggingGame())
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
#endif //ENGINE

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->GetModule<ModuleWindow>()->GetWindow());

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	LOG_VERBOSE("Destroying renderer");

	SDL_GL_DeleteContext(context);

	glDeleteBuffers(1, &uboCamera);

	glDeleteFramebuffers(1, &frameBuffer[0]);
	glDeleteTextures(1, &renderedTexture[0]);
#ifdef ENGINE
	glDeleteFramebuffers(1, &frameBuffer[1]);
	glDeleteTextures(1, &renderedTexture[1]);
#endif // ENGINE
	glDeleteFramebuffers(BLOOM_BLUR_PING_PONG, bloomBlurFramebuffers);
	glDeleteTextures(BLOOM_BLUR_PING_PONG, bloomBlurTextures);
	glDeleteRenderbuffers(1, &depthStencilRenderBuffer);
	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	App->GetModule<ModuleCamera>()->GetCamera()->SetAspectRatio(float(width) / height);
#ifdef ENGINE
	App->GetModule<ModuleEditor>()->Resized();
#endif // ENGINE
}

void ModuleRender::UpdateBuffers(unsigned width, unsigned height)
{
	gBuffer->InitGBuffer(width,height);
	
	// Shadow Map Depth Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer);

	glBindTexture(GL_TEXTURE_2D, gShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gShadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("G Framebuffer not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[0]);

	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRenderBuffer);

	glBindTexture(GL_TEXTURE_2D, renderedTexture[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}

#ifdef ENGINE
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[1]);

	glBindTexture(GL_TEXTURE_2D, renderedTexture[1]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture[1], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer auxiliar is not complete!");
	}
#endif // ENGINE

	for (unsigned int i = 0; i < BLOOM_BLUR_PING_PONG; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomBlurFramebuffers[i]);
		
		glBindTexture(GL_TEXTURE_2D, bloomBlurTextures[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomBlurTextures[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer bloom blur is not complete!");
		}
	}
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, parallelReductionInTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, width, height);
	glBindTexture(GL_TEXTURE_2D, parallelReductionOutTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, width, height);

	glBindTexture(GL_TEXTURE_2D, 0); //Unbind the texture
}

void ModuleRender::FillRenderList(const Quadtree* quadtree)
{
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	float3 cameraPos = camera->GetCamera()->GetPosition();

	if (camera->GetCamera()->IsInside(quadtree->GetBoundingBox()))
	{
		const std::set<GameObject*>& gameObjectsToRender = quadtree->GetGameObjects();
		if (quadtree->IsLeaf())
		{
			for (const GameObject* gameObject : gameObjectsToRender)
			{
				if (gameObject->IsActive() && gameObject->IsEnabled())
				{
					const ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();
					float dist = Length(cameraPos - transform->GetGlobalPosition());

					gameObjectsInFrustrum.insert(gameObject);
					objectsInFrustrumDistances[gameObject] = dist;
				}
			}
		}
		else if (!gameObjectsToRender.empty()) //If the node is not a leaf but has GameObjects shared by all children
		{
			for (const GameObject* gameObject : gameObjectsToRender)  //We draw all these objects
			{
				if (gameObject->IsActive() && gameObject->IsEnabled())
				{
					const ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();
					float dist = Length(cameraPos - transform->GetGlobalPosition());

					gameObjectsInFrustrum.insert(gameObject);
					objectsInFrustrumDistances[gameObject] = dist;
				}
			}

			FillRenderList(quadtree->GetFrontRightNode()); // And also call all the children to render
			FillRenderList(quadtree->GetFrontLeftNode());
			FillRenderList(quadtree->GetBackRightNode());
			FillRenderList(quadtree->GetBackLeftNode());
		}
		else
		{
			FillRenderList(quadtree->GetFrontRightNode());
			FillRenderList(quadtree->GetFrontLeftNode());
			FillRenderList(quadtree->GetBackRightNode());
			FillRenderList(quadtree->GetBackLeftNode());
		}
	}
}

void ModuleRender::AddToRenderList(const GameObject* gameObject)
{
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	float3 cameraPos = camera->GetCamera()->GetPosition();

	if (gameObject->GetParent() == nullptr)
	{
		return;
	}

	ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();
	// If an object doesn't have transform component it doesn't need to draw
	if (transform == nullptr)
	{
		return;
	}

	if (camera->GetCamera()->IsInside(transform->GetEncapsuledAABB()))
	{
		ComponentMeshRenderer* mesh = gameObject->GetComponentInternal<ComponentMeshRenderer>();
		if (gameObject->IsActive() && (mesh == nullptr || mesh->IsEnabled()))
		{
			const ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();
			float dist = Length(cameraPos - transform->GetGlobalPosition());

			gameObjectsInFrustrum.insert(gameObject);
			objectsInFrustrumDistances[gameObject] = dist;
		}
	}

	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* children : gameObject->GetChildren())
		{
			AddToRenderList(children);
		}
	}
}

void ModuleRender::DrawQuadtree(const Quadtree* quadtree)
{
#ifdef ENGINE
	if (quadtree->IsLeaf())
	{
		App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(quadtree->GetBoundingBox());
	}
	else
	{
		DrawQuadtree(quadtree->GetBackLeftNode());
		DrawQuadtree(quadtree->GetBackRightNode());
		DrawQuadtree(quadtree->GetFrontLeftNode());
		DrawQuadtree(quadtree->GetFrontRightNode());
	}
#endif // ENGINE
}

float2 ModuleRender::ParallelReduction(Program* program, int width, int height)
{
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("ComputeShader - Parallel Reduction"),
		"ComputeShader - Parallel Reduction");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer->GetDepthTexture());

	//Use the texture as an image
	glBindImageTexture(0, parallelReductionInTexture, 0, false, 0, GL_WRITE_ONLY, GL_RG32F);

	program->BindUniformInt2("inSize", width, height);

	unsigned int numGroupsX = (width + (8 - 1)) / 8;
	unsigned int numGroupsY = (height + (4 - 1)) / 4;

	glDispatchCompute(numGroupsX, numGroupsY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	GLuint srcTexture = parallelReductionInTexture; 
	GLuint dstTexture = parallelReductionOutTexture;

	while (numGroupsX > 1 || numGroupsY > 1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		//Use the texture as an image
		glBindImageTexture(0, dstTexture, 0, false, 0, GL_WRITE_ONLY, GL_RG32F);

		program->BindUniformInt2("inSize", numGroupsX, numGroupsY);

		numGroupsX = (numGroupsX + (8 - 1)) / 8;
		numGroupsY = (numGroupsY + (4 - 1)) / 4;

		glDispatchCompute(numGroupsX, numGroupsY, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		std::swap(srcTexture, dstTexture);
	}
	
	glPopDebugGroup();

	program->Deactivate();

	program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::MIN_MAX);

	program->Activate();
	
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("ComputeShader - Min Max"), "ComputeShader - Min Max");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	float2 minMax(0.0f, 0.0f);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, minMaxBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(minMax), &minMax[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, minMaxBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, minMaxBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(minMax), &minMax[0]);

	glPopDebugGroup();

	program->Deactivate();

	//glGetTextureSubImage(srcTexture, 0, 0, 0, 0, 1, 1, 1, GL_RG, GL_FLOAT, sizeof(float2), &minMax);

	glBindTexture(GL_TEXTURE_2D, 0);

	return minMax;
}

void ModuleRender::RenderShadowMap(const GameObject* light, const float2& minMax)
{
	// Get light position
	const ComponentTransform* lightTransform = light->GetComponent<ComponentTransform>();
	const float3& lightPos = lightTransform->GetGlobalPosition();

	// Compute camera frustrum bounding sphere
	math::Frustum* cameraFrustum = new Frustum(*App->GetModule<ModuleCamera>()->GetCamera()->GetFrustum());

	// SDSM: Final near an far distances
	float n = cameraFrustum->NearPlaneDistance();
	float f = cameraFrustum->FarPlaneDistance();
	float T = -(n + f) / (f - n);
	float S = (-2 * f * n) / (f - n);
	float distMin = S / (T + minMax[0]);
	float distMax = S / (T + minMax[1]);

	if (distMin > distMax) {
		distMin = 0.1f;
	}

	cameraFrustum->SetViewPlaneDistances(distMin, distMax);

	math::vec corners[8];
	cameraFrustum->GetCornerPoints(corners);

	float3 sumCorners(0.0f);

	for (unsigned int i = 0; i < 8; ++i)
	{
		sumCorners += corners[i];
	}

	const float3 sphereCenter = sumCorners.Div(8.0f);
	float sphereRadius = 0.0f;

	for (unsigned int i = 0; i < 8; ++i)
	{
		float distance = sphereCenter.Distance(corners[i]);
		if (distance > sphereRadius)
		{
			sphereRadius = distance;
		}
	}

	// Compute bounding box
	math::Frustum frustum;

	float3 lightFront = lightTransform->GetGlobalForward();
	float3 lightRight = Cross(lightFront, float3(0.0f, 1.0f, 0.0f)).Normalized();
	float3 lifghtUp = math::Cross(lightRight, lightFront).Normalized();
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetPos(sphereCenter - lightFront * sphereRadius);
	frustum.SetFront(lightFront);
	frustum.SetUp(lifghtUp);
	frustum.SetViewPlaneDistances(0.0f, sphereRadius * 2.0f);
	frustum.SetOrthographic(sphereRadius * 2.0f, sphereRadius * 2.0f);

	// Frustum culling with the created light frustum to obtain the meshes of the scene to take into account
	std::vector<GameObject*> objectsInFrustum =
		App->GetModule<ModuleScene>()->GetLoadedScene()->ObtainObjectsInFrustum(&frustum);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Shadow Mapping"), "Shadow Mapping");

	// Program binding
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SHADOW_MAPPING);
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	dirLightView = frustum.ViewMatrix();
	dirLightProj = frustum.ProjectionMatrix();

	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, &dirLightProj[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, &dirLightView[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	batchManager->DrawMeshes(objectsInFrustum, float3(frustum.Pos()));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	program->Deactivate();

	glPopDebugGroup();
}

void ModuleRender::DrawMeshes(std::vector<GameObject*>& objects, const float3& pos)
{
	batchManager->DrawMeshes(objects, pos);
}

void ModuleRender::DrawHighlight(GameObject* gameObject)
{
	std::queue<GameObject*> gameObjectQueue;
	gameObjectQueue.push(gameObject);

	while (!gameObjectQueue.empty())
	{
		const GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		std::vector<ComponentMeshRenderer*> meshes = currentGo->GetComponents<ComponentMeshRenderer>();
		
		if (gameObjectsInFrustrum.find(currentGo) != gameObjectsInFrustrum.end())
		{
			for (const ComponentMeshRenderer* mesh : meshes)
			{
				mesh->DrawHighlight();
			}
		}
	}
}

void ModuleRender::BindCameraToProgram(Program* program)
{
	program->Activate();

	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();
	float3 viewPos = App->GetModule<ModuleCamera>()->GetCamera()->GetPosition();

	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, &proj);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, &view);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	program->BindUniformFloat3("viewPos", viewPos);

	program->Deactivate();
}

void ModuleRender::BindCubemapToProgram(Program* program)
{
	program->Activate();

	Cubemap* cubemap = App->GetModule<ModuleScene>()->GetLoadedScene()->GetCubemap();
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->GetIrradiance());
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->GetPrefiltered());
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, cubemap->GetEnvironmentBRDF());

	program->BindUniformInt("numLevels_IBL", cubemap->GetNumMiMaps());
	program->BindUniformFloat("cubemap_intensity", cubemap->GetIntensity());

	program->Deactivate();
}

void ModuleRender::KawaseDualFiltering()
{
	// Blur bloom with kawase
	ModuleProgram* moduleProgram = App->GetModule<ModuleProgram>();
	bool kawaseFrameBuffer = true, firstIteration = true;
	int kawaseSamples = 10;
	Program* kawaseDownProgram = moduleProgram->GetProgram(ProgramType::KAWASE_DOWN);
	kawaseDownProgram->Activate();
	for (auto i = 0; i < kawaseSamples; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomBlurFramebuffers[kawaseFrameBuffer]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, firstIteration ? gBuffer->GetEmissiveTexture() : bloomBlurTextures[!kawaseFrameBuffer]);

		glDrawArrays(GL_TRIANGLES, 0, 3); // render Quad

		kawaseFrameBuffer = !kawaseFrameBuffer;
		if (firstIteration)
		{
			firstIteration = false;
		}
	}
	kawaseDownProgram->Deactivate();

	Program* kawaseUpProgram = moduleProgram->GetProgram(ProgramType::KAWASE_UP);
	kawaseUpProgram->Activate();
	for (auto i = 0; i < kawaseSamples; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloomBlurFramebuffers[kawaseFrameBuffer]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bloomBlurTextures[!kawaseFrameBuffer]);

		glDrawArrays(GL_TRIANGLES, 0, 3); // render Quad

		kawaseFrameBuffer = !kawaseFrameBuffer;
	}
	kawaseUpProgram->Deactivate();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ModuleRender::CheckIfTransparent(const GameObject* gameObject)
{
	const ComponentMeshRenderer* material = gameObject->GetComponentInternal<ComponentMeshRenderer>();
	if (material != nullptr && material->GetMaterial() != nullptr)
	{
		if (!material->GetMaterial()->IsTransparent())
			return false;
		else
			return true;
	}

	return false;
}
