#include "StdAfx.h"

#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModulePlayer.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "Cubemap/Cubemap.h"

#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentTransform.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Batch/BatchManager.h"
#include "DataModels/Skybox/Skybox.h"

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
	modelTypes({ "FBX" }),
	frameBuffer(0),
	renderedTexture(0),
	depthStencilRenderbuffer(0),
	vertexShader("default_vertex.glsl"),
	fragmentShader("default_fragment.glsl")
{
}

ModuleRender::~ModuleRender()
{
	delete batchManager;
	objectsInFrustrumDistances.clear();
	gameObjectsInFrustrum.clear();
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

	backgroundColor = float4(0.3f, 0.3f, 0.3f, 1.f);

	batchManager = new BatchManager();

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
	glDisable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW);	 // Front faces will be counter clockwise

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

#ifdef ENGINE
	glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &renderedTexture);
#endif // ENGINE
	glGenRenderbuffers(1, &depthStencilRenderbuffer);

	std::pair<int, int> windowSize = window->GetWindowSize();
	UpdateBuffers(windowSize.first, windowSize.second);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

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

	renderMapOpaque.clear();
	renderMapTransparent.clear();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);

	glViewport(0, 0, width, height);

	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00); // disable writing to the stencil buffer

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateRender", Optick::Category::Rendering);
#endif // DEBUG

	gameObjectsInFrustrum.clear();

	ModuleWindow* window = App->GetModule<ModuleWindow>();
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	ModuleDebugDraw* debug = App->GetModule<ModuleDebugDraw>();
	ModuleScene* scene = App->GetModule<ModuleScene>();
	ModulePlayer* modulePlayer = App->GetModule<ModulePlayer>();

	GameObject* player = modulePlayer->GetPlayer();

	Scene* loadedScene = scene->GetLoadedScene();

	const Skybox* skybox = loadedScene->GetSkybox();

	if (skybox)
	{
		skybox->Draw();
	}

	GameObject* goSelected = App->GetModule<ModuleScene>()->GetSelectedGameObject();

	bool isRoot = goSelected->GetParent() == nullptr;

	FillRenderList(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree());
	
	std::vector<GameObject*> nonStaticsGOs = App->GetModule<ModuleScene>()->GetLoadedScene()->GetNonStaticObjects();

	for (GameObject* nonStaticObj : nonStaticsGOs)
	{
		AddToRenderList(nonStaticObj);
	}
	
#ifndef ENGINE
	AddToRenderList(App->GetModule<ModulePlayer>()->GetPlayer());
#endif // !ENGINE

	AddToRenderList(goSelected);
	
	// Bind camera info to the shaders
	BindCameraToProgram(App->GetModule<ModuleProgram>()->GetProgram(ProgramType::DEFAULT));
	BindCameraToProgram(App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SPECULAR));

	//AddToRenderList(goSelected);
	
	if (App->GetModule<ModuleDebugDraw>()->IsShowingBoundingBoxes())
	{
		DrawQuadtree(loadedScene->GetRootQuadtree());
	}

	int w, h;
	SDL_GetWindowSize(window->GetWindow(), &w, &h);

	debug->Draw(camera->GetCamera()->GetViewMatrix(), camera->GetCamera()->GetProjectionMatrix(), w, h);

#ifdef ENGINE
	if (App->IsOnPlayMode())
	{
		AddToRenderList(player);
	}
#else
	if (player)
	{
		AddToRenderList(player);
	}
#endif // !ENGINE
	
	drawnGameObjects.clear();

	// -------- SCENE BATCH RENDERING -----------

	// Draw opaque objects
	glDepthFunc(GL_LEQUAL);
	batchManager->DrawOpaque(false);

	if (!App->IsOnPlayMode() && !isRoot)
	{
		// Draw selected opaque
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		batchManager->DrawOpaque(true);

		glPolygonMode(GL_FRONT, GL_FILL);
		glLineWidth(1);
		glDisable(GL_STENCIL_TEST);
	}

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
		glPolygonMode(GL_FRONT, GL_LINE);

		// Draw Highliht for selected objects
		DrawHighlight(goSelected);

		glPolygonMode(GL_FRONT, GL_FILL);
		glLineWidth(1);
		glDisable(GL_STENCIL_TEST);
	}

	glDisable(GL_BLEND);

	// -- DRAW ALL COMPONENTS IN THE FRUSTRUM --

	for (const GameObject* go : gameObjectsInFrustrum)
	{
		go->Draw();
	}

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	LOG_VERBOSE("Destroying renderer");

	SDL_GL_DeleteContext(context);

	glDeleteBuffers(1, &uboCamera);
	
#ifdef ENGINE
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &renderedTexture);
#endif // ENGINE
	glDeleteRenderbuffers(1, &depthStencilRenderbuffer);
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
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindRenderbuffer(GL_RENDERBUFFER, depthStencilRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRenderbuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ModuleRender::IsSupportedPath(const std::string& modelPath)
{
	bool valid = false;

	std::string format = modelPath.substr(modelPath.size() - 3);
	std::transform(format.begin(), format.end(), format.begin(), ::toupper);

	if (std::find(modelTypes.begin(), modelTypes.end(), format) != modelTypes.end())
	{
		valid = true;
	}

	return valid;
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
					const ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
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
					const ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
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

	ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
	// If an object doesn't have transform component it doesn't need to draw
	if (transform == nullptr)
	{
		return;
	}

	if (camera->GetCamera()->IsInside(transform->GetEncapsuledAABB()))
	{
		ComponentMeshRenderer* mesh = gameObject->GetComponent<ComponentMeshRenderer>();
		if (gameObject->IsActive() && (mesh == nullptr || mesh->IsEnabled()))
		{
			const ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();
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
	Cubemap* cubemap = App->GetModule<ModuleScene>()->GetLoadedScene()->GetCubemap();

	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, &proj);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, &view);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	program->BindUniformFloat3("viewPos", viewPos);

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

bool ModuleRender::CheckIfTransparent(const GameObject* gameObject)
{
	const ComponentMeshRenderer* material = gameObject->GetComponent<ComponentMeshRenderer>();
	if (material != nullptr && material->GetMaterial() != nullptr)
	{
		if (!material->GetMaterial()->IsTransparent())
			return false;
		else
			return true;
	}

	return false;
}