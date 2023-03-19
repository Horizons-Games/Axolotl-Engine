#include "EditorWindow.h"

#include "physfs.h"
#include "Application.h"
#include "../FileSystem/ModuleFileSystem.h"
#include "../FileSystem/UniqueID.h"
#include "../FileSystem/Json.h"





EditorWindow::EditorWindow(const std::string& name) : Window(name), flags(ImGuiWindowFlags_None),
	focused(false)
{}

EditorWindow::~EditorWindow()
{	
}

void EditorWindow::Start()
{
	load_meta(name.c_str(), enabled);
}

void EditorWindow::load_meta(std::string name, bool& enable)
{
	char* binaryBuffer = {};
	std::string lib = "Lib/StateWindow/" + name + ".meta";	
	
	if (App->fileSystem->Load(lib.c_str(), binaryBuffer) > 0) {


		rapidjson::Document doc;
		Json json(doc, doc);
		json.fromBuffer(binaryBuffer);
		std::string buffer = std::string(binaryBuffer);
		size_t i = buffer.find("State");
		size_t j = buffer.find("\n", i);
		if (i != std::string::npos)
		{
			if (buffer.substr(i + 8, j - (i + 8)) == "true")
			{
				enable = true;
			}
			else
			{
				enable = false;
			}

		}
	}
	else {
		enable = true;
	}	
	delete binaryBuffer;

}

void EditorWindow::Update_meta(bool enabled,std::string name)
{
	
		rapidjson::Document doc;
		Json json(doc, doc);
		json["UID"] = UniqueID::GenerateUID();
		json["Type"] = "Window";
		json["State"] = enabled;
		rapidjson::StringBuffer buffer;
		json.toBuffer(buffer);
		std::string lib = "Lib/StateWindow/" + name + ".meta";				
		App->fileSystem->Save(lib.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());	
}



void EditorWindow::Draw()
{	
	if (enabled)
	{
		if (ImGui::Begin(name.c_str(), &enabled, flags))
		{			

			ImGui::SetWindowSize(name.c_str(), GetStartingSize(), ImGuiCond_Once);

			DrawWindowContents();
			focused = ImGui::IsWindowFocused();			
		}
		ImGui::End();
	}		
	//The call to ImGui::Begin can change the value of io_enabled
	//so using "else" will cause the window to remain focused for an extra frame
	if (!enabled)
	{
		focused = false;
	}
	
}

void EditorWindow::CleanUp() 
{
	Update_meta(enabled, name);
}