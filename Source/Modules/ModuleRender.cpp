#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleEngineCamera.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"

#include "3DModels/Model.h"

#include <iostream>
#include <algorithm>

#include "Geometry/AABB.h"
#include "Geometry/Frustum.h"
#include "Math/float3x3.h"
#include "Math/float3.h"
		 
#include "GL/glew.h"

void __stdcall OurOpenGLErrorFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	const char* tmpSource = "", * tmpType = "", * tmpSeverity = "";
	switch (source) {
	case GL_DEBUG_SOURCE_API: tmpSource = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmpSource = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: tmpSource = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: tmpSource = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: tmpSource = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER: tmpSource = "Other"; break;
	};
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: tmpType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmpType = "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmpType = "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: tmpType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: tmpType = "Performance"; break;
	case GL_DEBUG_TYPE_MARKER: tmpType = "Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: tmpType = "Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: tmpType = "Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: tmpType = "Other"; break;
	};
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: tmpSeverity = "high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: tmpSeverity = "medium"; break;
	case GL_DEBUG_SEVERITY_LOW: tmpSeverity = "low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: tmpSeverity = "notification"; break;
	};
}


ModuleRender::ModuleRender()
{
	this->context = nullptr;
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	ENGINE_LOG("--------- Render Init ----------");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // we want to have a stencil buffer with 8 bits

	SDL_GetVersion(&sdlVersion);

	this->cacheLineSize = SDL_GetCPUCacheLineSize();
	this->cpuCount = SDL_GetCPUCount();
	this->ram = SDL_GetSystemRAM();

	// CAPS
	if (SDL_Has3DNow()) sprintf_s(caps, 75, "%s3DNow, ", caps);
	if (SDL_HasAltiVec()) sprintf_s(caps, 75, "%sAltiVec, ", caps);
	if (SDL_HasAVX()) sprintf_s(caps, 75, "%sAVX, ", caps);
	if (SDL_HasAVX2()) sprintf_s(caps, 75, "%sAVX2, ", caps);
	if (SDL_HasMMX()) sprintf_s(caps, 75, "%sMMX, ", caps);
	if (SDL_HasRDTSC()) sprintf_s(caps, 75, "%sRDTSC, ", caps);
	if (SDL_HasSSE()) sprintf_s(caps, 75, "%sSSE, ", caps);
	if (SDL_HasSSE2()) sprintf_s(caps, 75, "%sSSE2, ", caps);
	if (SDL_HasSSE3()) sprintf_s(caps, 75, "%sSSE3, ", caps);
	if (SDL_HasSSE41()) sprintf_s(caps, 75, "%sSSE41, ", caps);
	if (SDL_HasSSE42()) sprintf_s(caps, 75, "%sSSE42, ", caps);

	this->context = SDL_GL_CreateContext(App->window->window);

	this->backgroundColor = float4(0.3f, 0.3f, 0.3f, 1.f);

	GLenum err = glewInit();
	ENGINE_LOG("glew error %s", glewGetErrorString(err));
	// … check for errors
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

	return true;
}

bool ModuleRender::Start()
{
	ENGINE_LOG("--------- Render Start ----------");

	const char* vertexSource = App->program->LoadShaderSource("Assets/Shaders/default_vertex.glsl");
	const char* fragmentSource = App->program->LoadShaderSource("Assets/Shaders/default_fragment.glsl");

	unsigned vertexShader = App->program->CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned fragmentShader = App->program->CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	delete vertexSource;
	delete fragmentSource;

	App->program->CreateProgram(vertexShader, fragmentShader);

	Model* bakerHouse = new Model;
	bakerHouse->Load("Assets/Models/BakerHouse.fbx");

	models.push_back(bakerHouse);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	int width, height;

	SDL_GetWindowSize(App->window->window, &width, &height);

	glViewport(0, 0, width, height);

	glClearColor(this->backgroundColor.x, this->backgroundColor.y, 
				 this->backgroundColor.z, this->backgroundColor.w);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	for (Model* model : models)
	{
		model->Draw();
	}

	int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);

	App->debug->Draw(App->engineCamera->GetViewMatrix(),
		App->engineCamera->GetProjectionMatrix(), w, h);

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	ENGINE_LOG("Destroying renderer");

	SDL_GL_DeleteContext(this->context);

	glDeleteBuffers(1, &this->vbo);

	for (Model* model : models)
	{
		delete model;
	}

	models.clear();

	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
	App->engineCamera->SetAspectRatio(float(width) / height);
	App->editor->Resized();
}

void ModuleRender::SetBackgroundColor(float4 color)
{
	backgroundColor = color;
}

float4 ModuleRender::GetBackgroundColor() const
{
	return backgroundColor;
}

Model* ModuleRender::GetModel(unsigned pos) const
{
	return models[pos];
}

const SDL_version& ModuleRender::GetSDLVersion() const
{
	return this->sdlVersion;
}

int ModuleRender::GetCPUCount() const
{
	return this->cpuCount;
}

int ModuleRender::GetCacheLineSize() const
{
	return this->cacheLineSize;
}

float ModuleRender::GetRamGB() const
{
	return (float)this->ram/1024.0f;
}

const char* ModuleRender::GetCaps() const
{
	return this->caps;
}

bool ModuleRender::LoadModel(const char* path)
{
	ENGINE_LOG("---- Loading Model ----");

	Model* newModel = new Model;
	newModel->Load(path);

	if (AnyModelLoaded())
	{
		delete models[0];
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
