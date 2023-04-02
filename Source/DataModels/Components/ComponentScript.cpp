#include "ComponentScript.h"
#include "Application.h"


ComponentScript::ComponentScript(bool active, GameObject* owner, const char* path): Component(ComponentType::SCRIPT, active, owner, false){
	this->path = path;
}

ComponentScript::~ComponentScript() {

}

void ComponentScript::LoadLibraryScript(){
	this->library = LoadLibraryA(this->path);
	this->scriptInit = (initScript)GetProcAddress(this->library, "Init()");
	if (this->scriptInit == 0) ENGINE_LOG("Error loading script");
	this->scriptUpdate = (updateScript)GetProcAddress(this->library, "Update()");
	if (this->scriptUpdate == 0) ENGINE_LOG("Error loading script");
}

void ComponentScript::FreeLibraryScript() {
	FreeLibrary(this->library);
	this->scriptInit = nullptr;
	this->scriptUpdate = nullptr;
}

void ComponentScript::Init()
{
	this->LoadLibraryScript();
	this->scriptInit(this->owner);
}


void ComponentScript::Update()
{
	this->scriptUpdate(this->owner);
}
