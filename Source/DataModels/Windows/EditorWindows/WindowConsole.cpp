#include "WindowConsole.h"

WindowConsole::WindowConsole() : EditorWindow("Console")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowConsole::~WindowConsole()
{
}

void WindowConsole::DrawWindowContents()
{
	for (int i = 0; i < consloneContents.size(); ++i)
	{
		const char* line = consloneContents[i];
		ImGui::TextUnformatted(line);
	}
}

void WindowConsole::Output(const char* textToPrint)
{
	//APP_LOG(textToPrint);
	consloneContents.push_back(_strdup(textToPrint));
}
