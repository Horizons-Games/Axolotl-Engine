#include "WindowFileBrowser.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/GameObject/GameObject.h"
#include "Auxiliar/Utils/ConvertU8String.h"

#include "Timer/Timer.h"
WindowFileBrowser::WindowFileBrowser() : EditorWindow("File Browser"),
	title(ConvertU8String(ICON_IGFD_FOLDER) + " Import Asset"),
	dialogName("Choose File"),
	filters(".*"),
	startPath("."),
	isLoading(false),
	timer(nullptr),
	winLoading(std::make_unique<WindowLoading>()),
	browserPath(fileDialogBrowser.GetCurrentPath() + "Assets")
{
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".cpp",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".h",
		ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".hpp",
		ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".md",
		ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".png",
		ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE_PIC));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByExtention, ".gif",
		ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr,
		ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ConvertU8String(ICON_IGFD_FOLDER));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt",
		ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ConvertU8String(ICON_IGFD_ADD));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByFullName, "doc",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE_PIC));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_BOOKMARK));
	fileDialogBrowser.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ConvertU8String(ICON_IGFD_SAVE));

	fileDialogImporter.SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".cpp",
		ImVec4(1.0f, 1.0f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".h",
		ImVec4(0.0f, 1.0f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".hpp",
		ImVec4(0.0f, 0.0f, 1.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".md",
		ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".png",
		ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE_PIC));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByExtention, ".gif",
		ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]");
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeDir, nullptr,
		ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ConvertU8String(ICON_IGFD_FOLDER));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt",
		ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ConvertU8String(ICON_IGFD_ADD));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByFullName, "doc",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_FILE_PIC));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ConvertU8String(ICON_IGFD_BOOKMARK));
	fileDialogImporter.SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git",
		ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ConvertU8String(ICON_IGFD_SAVE));
}

WindowFileBrowser::~WindowFileBrowser()
{
}

void WindowFileBrowser::SaveAsWindow(bool& isSaving)
{
	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	std::string sceneName = App->scene->GetLoadedScene()->GetRoot()->GetName();
	if (!App->fileSystem->Exists(("Assets/Scenes/" + sceneName + SCENE_EXTENSION).c_str()))
	{
		Uint32 flags = ImGuiFileDialogFlags_Modal;
		if (isSave)
		{
			flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
		}
		fileDialogImporter.OpenDialog("ChooseFileDlgKey", dialogName.c_str(), filters.c_str(), startPath.c_str(),
			"", 1, nullptr, flags);
	}
	if (fileDialogImporter.Display("ChooseFileDlgKey"))
	{
		if (fileDialogImporter.IsOk())
		{
			DoThisIfOk();
		}
		isSaving = false;
		fileDialogImporter.Close();
	}

}

void WindowFileBrowser::DrawWindowContents()
{
	ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
	if (ImGui::Button(title.c_str()))
	{
		Uint32 flags = ImGuiFileDialogFlags_Modal;
		if (isSave)
		{
			flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
		}
		fileDialogImporter.OpenDialog("ChooseFileDlgKey", dialogName.c_str(), filters.c_str(), startPath.c_str(),
			"", 1, nullptr, flags);
	}
	
	if (fileDialogImporter.Display("ChooseFileDlgKey"))
	{
		if (fileDialogImporter.IsOk())
		{
			isLoading = true;
			DoThisIfOk();
		}
		fileDialogImporter.Close();
	}
	
	if (title == ConvertU8String(ICON_IGFD_FOLDER) + " Import Asset")
	{
		Browser();
	}

	ImportResourceWithLoadingWindow();

	if (futureResource.valid() && !isLoading)
	{
		GetResourceAfterImport();
	}
}

void WindowFileBrowser::Browser()
{
	fileDialogBrowser.OpenDialog("embedded", "Select File", ".*",browserPath, "", -1, nullptr,
		ImGuiFileDialogFlags_NoDialog |
		ImGuiFileDialogFlags_DisableBookmarkMode |
		ImGuiFileDialogFlags_DisableCreateDirectoryButton);
	fileDialogBrowser.Display("embedded", ImGuiWindowFlags_NoCollapse, ImVec2(0, 0), ImVec2(0, 350));
		
	if (std::string::npos == fileDialogBrowser.GetCurrentPath().find("Assets"))
	{
		fileDialogBrowser.Close();
	}
	
}

void WindowFileBrowser::DoThisIfOk()
{
	filePathName = fileDialogImporter.GetFilePathName();
	this->ImportResourceAsync(filePathName);
}

void WindowFileBrowser::ImportResourceWithLoadingWindow()
{
	winLoading->Draw(isLoading);
	if (isLoading && futureResource._Is_ready() && timer->Read() > 1000)
	{
		isLoading = false;
		timer->Stop();
		ENGINE_LOG("Resource loaded succesfully");
	}
}

void WindowFileBrowser::ImportResourceAsync(const std::string& filePath)
{
	futureResource = App->resources->ImportThread(filePath);

	timer = std::make_unique<Timer>();
	timer->Start();
	ENGINE_LOG("Started loading resource");
}

void WindowFileBrowser::GetResourceAfterImport()
{
}
