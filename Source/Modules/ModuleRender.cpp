#include "ModuleRender.h"

#include <queue>

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "DataModels/Skybox/Skybox.h"
#include "Scene/Scene.h"
#include "Components/ComponentTransform.h"
#include "DataModels/Resources/ResourceMaterial.h"
#include "DataStructures/Quadtree.h"
#include "Components/ComponentMeshRenderer.h"

#include "DataModels/GameObject/GameObject.h"

#include "Components/ComponentTransform.h"
#ifdef DEBUG
#include "optick.h"
#endif // DEBUG

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, 
GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmpSource = "", * tmpType = "", * tmpSeverity = "";

	switch (source) {
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

	switch (type) {
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

	switch (severity) {
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

ModuleRender::ModuleRender() : context(nullptr), modelTypes({ "FBX" }), frameBuffer(0), renderedTexture(0), 
	depthStencilRenderbuffer(0),
	vertexShader("default_vertex.glsl"), fragmentShader("default_fragment.glsl")
{
}

ModuleRender::~ModuleRender()
{
}

bool ModuleRender::Init()
{
	ENGINE_LOG("--------- Render Init ----------");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	context = SDL_GL_CreateContext(App->GetModule<ModuleWindow>()->GetWindow());

	backgroundColor = float4(0.3f, 0.3f, 0.3f, 1.f);

	GLenum err = glewInit();
	ENGINE_LOG("glew error %s", glewGetErrorString(err));
	// � check for errors
	ENGINE_LOG("Using Glew %s", glewGetString(GLEW_VERSION));
	// Should be 2.0

	ENGINE_LOG("Vendor: %s", glGetString(GL_VENDOR));
	ENGINE_LOG("Renderer: %s", glGetString(GL_RENDERER));
	ENGINE_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	ENGINE_LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEBUG_OUTPUT); 
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
	glDebugMessageCallback(&OurOpenGLErrorFunction, nullptr); 
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);

	glEnable(GL_DEPTH_TEST);	// Enable depth test
	glDisable(GL_CULL_FACE);	// Enable cull backward faces
	glFrontFace(GL_CCW);		// Front faces will be counter clockwise

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef ENGINE
	glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &renderedTexture);
#endif // ENGINE
	glGenRenderbuffers(1, &depthStencilRenderbuffer);

	std::pair<int, int> windowSize = App->GetModule<ModuleWindow>()->GetWindowSize();
	UpdateBuffers(windowSize.first, windowSize.second);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	return true;
}

update_status ModuleRender::PreUpdate()
{
	int width, height;

	//opaqueGOToDraw.clear();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);

	glViewport(0, 0, width, height);

	glClearColor(backgroundColor.x, backgroundColor.y, 
				 backgroundColor.z, backgroundColor.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00); // disable writing to the stencil buffer
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateRender", Optick::Category::Rendering);
#endif // DEBUG
	opaqueGOToDraw.clear();
	transparentGOToDraw.clear();

	const Skybox* skybox = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSkybox();
	if (skybox)
	{
		skybox->Draw();
	}

	if (App->GetModule<ModuleDebugDraw>()->IsShowingBoundingBoxes())
	{
		DrawQuadtree(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree());
	}

	int w, h;
	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &w, &h);

	App->GetModule<ModuleDebugDraw>()->Draw(App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix(),
		App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix(), w, h);

	GameObject* goSelected = App->GetModule<ModuleScene>()->GetSelectedGameObject();

	bool isRoot = goSelected->GetParent() == nullptr;

	FillRenderList(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree());
	std::vector<GameObject*> nonStaticsGOs = App->GetModule<ModuleScene>()->GetLoadedScene()->GetNonStaticObjects();
	for (GameObject* nonStaticObj : nonStaticsGOs)
	{
		AddToRenderList(nonStaticObj);
	}

#ifdef ENGINE
	if (App->IsOnPlayMode())
	{
		AddToRenderList(App->GetModule<ModulePlayer>()->GetPlayer());
	}
#else
	if (App->GetModule<ModulePlayer>()->GetPlayer())
	{
		AddToRenderList(App->GetModule<ModulePlayer>()->GetPlayer());
	}
#endif // !ENGINE
	
	if (isRoot) 
	{
		opaqueGOToDraw.push_back(goSelected);
	}
	else
	{
		AddToRenderList(goSelected);
	}

	drawnGameObjects.clear();

	//Draw opaque
	glDepthFunc(GL_LEQUAL);
	for (const GameObject* gameObject : opaqueGOToDraw)
	{
		DrawGameObject(gameObject);
	}

	// Draw Transparent
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (std::map<float, const GameObject*>::reverse_iterator it = transparentGOToDraw.rbegin(); it != transparentGOToDraw.rend(); ++it)
	{	
		DrawGameObject((*it).second);
	}
	glDisable(GL_BLEND);

#ifndef ENGINE
	if (!App->IsDebuggingGame())
	{
		return update_status::UPDATE_CONTINUE;
	}
#endif //ENGINE


	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->GetModule<ModuleWindow>()->GetWindow());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	ENGINE_LOG("Destroying renderer");

	SDL_GL_DeleteContext(context);

	glDeleteBuffers(1, &vbo);
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
		ENGINE_LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
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
	float3 cameraPos = App->GetModule<ModuleCamera>()->GetCamera()->GetPosition();

	if (App->GetModule<ModuleCamera>()->GetCamera()->IsInside(quadtree->GetBoundingBox()))
	{
		const std::set<GameObject*>& gameObjectsToRender = quadtree->GetGameObjects();
		if (quadtree->IsLeaf()) 
		{
			for (const GameObject* gameObject : gameObjectsToRender)
			{
				if (gameObject->IsEnabled())
				{
					if (!CheckIfTransparent(gameObject))
						opaqueGOToDraw.push_back(gameObject);
					else
					{
						const ComponentTransform* transform = 
							static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
						float dist = Length(cameraPos - transform->GetGlobalPosition());
						while (transparentGOToDraw[dist] != nullptr) //If an object is at the same position as another one
						{ 
							float addDistance = 0.0001f;
							dist += addDistance;
						}
						transparentGOToDraw[dist] = gameObject;
					}
				}
			}
		}
		else if (!gameObjectsToRender.empty()) //If the node is not a leaf but has GameObjects shared by all children
		{
			for (const GameObject* gameObject : gameObjectsToRender)  //We draw all these objects
			{
				if (gameObject->IsEnabled())
				{
					if (!CheckIfTransparent(gameObject))
						opaqueGOToDraw.push_back(gameObject);
					else
					{
						const ComponentTransform* transform = 
							static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
						float dist = Length(cameraPos - transform->GetGlobalPosition());
						while (transparentGOToDraw[dist] != nullptr) 
						{
							float addDistance = 0.0001f;
							dist += addDistance;
						}
						transparentGOToDraw[dist] = gameObject;
					}
				}
			}
			FillRenderList(quadtree->GetFrontRightNode()); //And also call all the children to render
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

void ModuleRender::AddToRenderList(GameObject* gameObject)
{
	float3 cameraPos = App->GetModule<ModuleCamera>()->GetCamera()->GetPosition();

	if (gameObject->GetParent() == nullptr)
	{
		return;
	}

	ComponentTransform* transform = static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
	//If an object doesn't have transform component it doesn't need to draw
	if (transform == nullptr)
	{
		return;
	}

	if (App->GetModule<ModuleCamera>()->GetCamera()->IsInside(transform->GetEncapsuledAABB()))
	{
		if (gameObject->IsEnabled())
		{
			if (!CheckIfTransparent(gameObject))
				opaqueGOToDraw.push_back(gameObject);
			else
			{
				const ComponentTransform* transform =
					static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
				float dist = Length(cameraPos - transform->GetGlobalPosition());
				while (transparentGOToDraw[dist] != nullptr) 
				{
					float addDistance = 0.0001f;
					dist += addDistance;
				}
				transparentGOToDraw[dist] = gameObject;
			}
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

void ModuleRender::DrawGameObject(const GameObject* gameObject)
{
	if (std::find(std::begin(drawnGameObjects), std::end(drawnGameObjects), gameObject->GetUID()) !=
		std::end(drawnGameObjects))
	{
		return;
	}

	GameObject* goSelected = App->GetModule<ModuleScene>()->GetSelectedGameObject();

	if (gameObject != nullptr && gameObject->IsActive())
	{
		if (goSelected->GetParent() != nullptr && gameObject == goSelected && (!App->IsOnPlayMode() || SDL_ShowCursor(SDL_QUERY)))
		{
			DrawSelectedHighlightGameObject(goSelected);
		}
		else
		{
			gameObject->Draw();
			drawnGameObjects.push_back(gameObject->GetUID());
		}
	}
}

void ModuleRender::DrawSelectedHighlightGameObject(GameObject* gameObject)
{
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
	glStencilMask(0xFF); // enable writing to the stencil buffer
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	DrawSelectedAndChildren(gameObject);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //discard the ones that are previously captured
	glLineWidth(25);
	glPolygonMode(GL_FRONT, GL_LINE);
	DrawHighlight(gameObject);
	glPolygonMode(GL_FRONT, GL_FILL);
	glLineWidth(1);
	glDisable(GL_STENCIL_TEST);
}

void ModuleRender::DrawSelectedAndChildren(GameObject* gameObject)
{
	std::queue<GameObject*> gameObjectQueue;
	gameObjectQueue.push(gameObject);
	while (!gameObjectQueue.empty())
	{
		GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		currentGo->Draw();
		drawnGameObjects.push_back(gameObject->GetUID());
	}
}

void ModuleRender::DrawHighlight(GameObject* gameObject)
{
	std::queue<GameObject*> gameObjectQueue;
	gameObjectQueue.push(gameObject);
	while (!gameObjectQueue.empty())
	{
		GameObject* currentGo = gameObjectQueue.front();
		gameObjectQueue.pop();
		for (GameObject* child : currentGo->GetChildren())
		{
			if (child->IsEnabled())
			{
				gameObjectQueue.push(child);
			}
		}
		std::vector<ComponentMeshRenderer*> meshes =
			currentGo->GetComponentsByType<ComponentMeshRenderer>(ComponentType::MESHRENDERER);
		for (ComponentMeshRenderer* mesh : meshes)
		{
			mesh->DrawHighlight();
		}
	}
}

bool ModuleRender::CheckIfTransparent(const GameObject* gameObject)
{
	ComponentMeshRenderer* material = static_cast<ComponentMeshRenderer*>(gameObject->GetComponent(ComponentType::MESHRENDERER));
	if (material != nullptr && material->GetMaterial() != nullptr)
	{
		if (!material->GetMaterial()->GetTransparent())
			return false;
		else
			return true;
	}

	return false;
}
