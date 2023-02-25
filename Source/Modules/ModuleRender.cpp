#pragma warning (disable: 26495)

#include "Globals.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "DataModels/Resources/ResourceModel.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "FileSystem/ModuleFileSystem.h"
#include "DataModels/Resources/ResourceSkyBox.h"
#include "DataModels/Skybox/Skybox.h"
#include "Scene/Scene.h"

#include "GameObject/GameObject.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentBoundingBoxes.h"

#include "optick.h"

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
	depthRenderBuffer(0), vertexShader("default_vertex.glsl"), fragmentShader("default_fragment.glsl")
{
}

ModuleRender::~ModuleRender()
{
}

bool ModuleRender::Init()
{
	ENGINE_LOG("--------- Render Init ----------");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	context = SDL_GL_CreateContext(App->window->GetWindow());

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
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,		// stencil fail
				GL_KEEP,		// stencil pass, depth fail
				GL_REPLACE);	// stencil pass, depth pass
	glDisable(GL_CULL_FACE);	// Enable cull backward faces
	glFrontFace(GL_CCW);		// Front faces will be counter clockwise

	glEnable(GL_TEXTURE_2D);

	glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &renderedTexture);
	glGenRenderbuffers(1, &depthStencilRenderbuffer);

	std::pair<int, int> windowSize = App->window->GetWindowSize();
	UpdateBuffers(windowSize.first, windowSize.second);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	return true;
}

bool ModuleRender::Start()
{
	ENGINE_LOG("--------- Render Start ----------");

	UpdateProgram();

#if !defined(GAME)
	UID skyboxUID = App->resources->ImportResource("Assets/Skybox/skybox.sky");
#else
	UID skyboxUID = App->resources->GetSkyBoxResource();
#endif
	std::shared_ptr<ResourceSkyBox> resourceSkybox = 
		std::dynamic_pointer_cast<ResourceSkyBox>(App->resources->RequestResource(skyboxUID).lock());
	if (resourceSkybox)
	{
		skybox = std::make_unique<Skybox>(resourceSkybox);
	}
	return true;
}

update_status ModuleRender::PreUpdate()
{
	int width, height;

	gameObjectsToDraw.clear();

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	SDL_GetWindowSize(App->window->GetWindow(), &width, &height);

	glViewport(0, 0, width, height);

	glClearColor(backgroundColor.x, backgroundColor.y, 
				 backgroundColor.z, backgroundColor.w);

	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00); // disable writing to the stencil buffer
	return UPDATE_CONTINUE;
}

update_status ModuleRender::Update()
{
	OPTICK_CATEGORY("UpdateRender", Optick::Category::Rendering);

	if (skybox)
	{
		skybox->Draw();
	}

	gameObjectsToDraw.clear();

	GameObject* goSelected = App->scene->GetSelectedGameObject();

	bool isRoot = goSelected->GetParent() == nullptr;

	FillRenderList(App->scene->GetLoadedScene()->GetSceneQuadTree());

	if (isRoot) 
	{
		gameObjectsToDraw.push_back(goSelected);
	}
	for (const GameObject* gameObject : gameObjectsToDraw)
	{
		if (gameObject != nullptr && gameObject->IsActive())
			gameObject->Draw();
	}

	if (!isRoot && goSelected != nullptr && goSelected->IsActive()) 
	{
		glEnable(GL_STENCIL_TEST);   // Nos aseguramos que está habilitado
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
		glStencilMask(0xFF); // enable writing to the stencil buffer
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Aquí tienes dos opciones, o no deshabilitas depth_test cuando llamas a DrawHighLight o, si lo haces, el segundo parámetro debería ser GL_REPLACE (mi código no deshabilita el depth_test)
		goSelected->DrawSelected();

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //discard the ones that are previously captured
		glLineWidth(25);   // Esto es un truco que te permite no escalar el modelo cuando llamas a DrawHighlight, consiste en pintar lineas en vez de triangulos con un ancho grande que generará el Highlight
		glPolygonMode(GL_FRONT, GL_LINE); // Pintamos lineas en vez de triangulos
		goSelected->DrawHighlight();  // DrawHighlight sin escalas
		glPolygonMode(GL_FRONT, GL_FILL); // Volvemos a pintar trinagulos
		glLineWidth(1);  // Las Lineas vuelven a su tamaño original
	}

	AddToRenderList(goSelected);

	if(App->debug->IsShowingBoundingBoxes())DrawQuadtree(App->scene->GetLoadedScene()->GetSceneQuadTree());

	int w, h;
	SDL_GetWindowSize(App->window->GetWindow(), &w, &h);

	App->debug->Draw(App->engineCamera->GetViewMatrix(),
	App->engineCamera->GetProjectionMatrix(), w, h);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->GetWindow());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	ENGINE_LOG("Destroying renderer");

	SDL_GL_DeleteContext(context);

	glDeleteBuffers(1, &vbo);
	
	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	App->engineCamera->SetAspectRatio(float(width) / height);
	App->editor->Resized();
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
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		ENGINE_LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::SetShaders(const std::string& vertexShader, const std::string& fragmentShader)
{
	this->vertexShader = vertexShader.c_str();
	this->fragmentShader = fragmentShader.c_str();
	UpdateProgram();
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

void ModuleRender::UpdateProgram()
{
	char* vertexSource;
	char * fragmentSource;
	App->fileSystem->Load(("Lib/Shaders/" + vertexShader).c_str(), vertexSource);
	App->fileSystem->Load(("Lib/Shaders/" + fragmentShader).c_str(), fragmentSource);
	unsigned vertexShader = App->program->CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned fragmentShader = App->program->CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	delete vertexSource;
	delete fragmentSource;

	App->program->CreateProgram(vertexShader, fragmentShader);
}

void ModuleRender::FillRenderList(const Quadtree* quadtree)
{
	if (App->engineCamera->IsInside(quadtree->GetBoundingBox()) || 
		App->scene->GetLoadedScene()->IsInsideACamera(quadtree->GetBoundingBox()))
	{
		const std::list<const GameObject*>& gameObjectsToRender = quadtree->GetGameObjects();
		if (quadtree->IsLeaf()) 
		{
			for (const GameObject* gameObject : gameObjectsToRender)
			{
				gameObjectsToDraw.push_back(gameObject);
			}
		}
		else if (!gameObjectsToRender.empty()) //If the node is not a leaf but has GameObjects shared by all children
		{
			for (const GameObject* gameObject : gameObjectsToRender)  //We draw all these objects
			{
				gameObjectsToDraw.push_back(gameObject);
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

void ModuleRender::AddToRenderList(const GameObject* gameObject)
{
	ComponentBoundingBoxes* boxes =
		static_cast<ComponentBoundingBoxes*>(gameObject->GetComponent(ComponentType::BOUNDINGBOX));

	if (App->engineCamera->IsInside(boxes->GetEncapsuledAABB())
		|| App->scene->GetLoadedScene()->IsInsideACamera(boxes->GetEncapsuledAABB()))
	{
		gameObjectsToDraw.push_back(gameObject);
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
	if (quadtree->IsLeaf())
	{
		App->debug->DrawBoundingBox(quadtree->GetBoundingBox());
	}
	else
	{
		DrawQuadtree(quadtree->GetBackLeftNode());
		DrawQuadtree(quadtree->GetBackRightNode());
		DrawQuadtree(quadtree->GetFrontLeftNode());
		DrawQuadtree(quadtree->GetFrontRightNode());
	}
	
}

