#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "Quadtree.h"

#include "3DModels/Model.h"

#include <iostream>
#include <algorithm>

#include "Geometry/AABB.h"
#include "Geometry/Frustum.h"
#include "Math/float3x3.h"
#include "Math/float3.h"

#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMesh.h"
		 
#include "GL/glew.h"

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

ModuleRender::ModuleRender()
{
	this->context = nullptr;
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

	this->context = SDL_GL_CreateContext(App->window->GetWindow());

	this->backgroundColor = float4(0.3f, 0.3f, 0.3f, 1.f);

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

	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDisable(GL_CULL_FACE); // Enable cull backward faces
	glFrontFace(GL_CCW); // Front faces will be counter clockwise

	glEnable(GL_TEXTURE_2D);

	glGenFramebuffers(1, &frameBuffer);
	glGenTextures(1, &renderedTexture);
	glGenRenderbuffers(1, &depthRenderBuffer);

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

	std::shared_ptr<Model> bakerHouse = std::make_shared<Model>();
	bakerHouse->Load("Assets/Models/BakerHouse.fbx");

	models.push_back(bakerHouse);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	int width, height;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	SDL_GetWindowSize(App->window->GetWindow(), &width, &height);

	glViewport(0, 0, width, height);

	glClearColor(this->backgroundColor.x, this->backgroundColor.y, 
				 this->backgroundColor.z, this->backgroundColor.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::Update()
{
	for (std::shared_ptr<Model> model : models)
	{
		model->Draw();
	}

	/* 
	*Logic to apply when model class is deleted and GameObjects are implemented
	*
	
	FIRST APPROACH
	DrawScene(App->scene->GetSceneQuadTree());
	
	
	SECOND APPROACH
	const std::list<GameObject*>& gameObjectsToDraw = 
		App->scene->GetSceneQuadTree()->GetGameObjectsToDraw();
	for (GameObject* gameObject : gameObjectsToDraw) 
	{
		for (Component* component : gameObject->GetComponents()) 
		{
			if (component->GetType() == ComponentType::MESH) 
			{
				//Draw gameobject
			}
		}
	}
	*/
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

	SDL_GL_DeleteContext(this->context);

	glDeleteBuffers(1, &this->vbo);

	models.clear();

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

	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRender::SetShaders(const std::string& vertexShader, const std::string& fragmentShader)
{
	this->vertexShader = vertexShader.c_str();
	this->fragmentShader = fragmentShader.c_str();
	UpdateProgram();
}

bool ModuleRender::LoadModel(const char* path)
{
	ENGINE_LOG("---- Loading Model ----");

	std::shared_ptr<Model> newModel = std::make_shared<Model>();
	newModel->Load(path);

	if (AnyModelLoaded())
	{
		models[0] = nullptr;
		models.clear();
	}

	models.push_back(newModel);

	return false;
}

bool ModuleRender::AnyModelLoaded()
{
	return !models.empty();
}

bool ModuleRender::IsSupportedPath(const std::string& modelPath)
{
	bool valid = false;

	std::string format = modelPath.substr(modelPath.size() - 3);
	std::transform(format.begin(), format.end(), format.begin(), ::toupper);

	if (std::find(this->modelTypes.begin(), this->modelTypes.end(), format) != this->modelTypes.end())
	{
		valid = true;
	}

	return valid;
}

void ModuleRender::UpdateProgram()
{
	const char* vertexSource = App->program->LoadShaderSource(("Assets/Shaders/" + this->vertexShader).c_str());
	const char* fragmentSource = App->program->LoadShaderSource(("Assets/Shaders/" + this->fragmentShader).c_str());

	unsigned vertexShader = App->program->CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned fragmentShader = App->program->CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	delete vertexSource;
	delete fragmentSource;

	App->program->CreateProgram(vertexShader, fragmentShader);
}

void ModuleRender::DrawScene(Quadtree* quadtree)
{
	if (App->engineCamera->IsInside(quadtree->GetBoundingBox())) 
	{
		auto gameObjectsToRender = quadtree->GetGameObjects();
		if (quadtree->IsLeaf() || !gameObjectsToRender.empty()) 
		{
			for (GameObject* gameObject : gameObjectsToRender)
			{
				//gameObject->Draw;
			}
		}
		else 
		{
			DrawScene(quadtree->GetFrontRightNode());
			DrawScene(quadtree->GetFrontLeftNode());
			DrawScene(quadtree->GetBackRightNode());
			DrawScene(quadtree->GetBackLeftNode());
		}
	}
}
