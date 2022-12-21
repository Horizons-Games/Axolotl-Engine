#include <stdlib.h>
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"

#pragma comment( lib, "SDL/lib/x64/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x64/SDL2main.lib" )

enum main_states
{
	MAIN_CREATION,  
	MAIN_INIT,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

std::unique_ptr<Application> App;
EngineLog* engineLog = new EngineLog();

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			ENGINE_LOG("Application Creation --------------");
			App = std::make_unique<Application>();
			state = MAIN_INIT;
			break;

		case MAIN_INIT:
			ENGINE_LOG("Application Init --------------");
			if (App->Init() == false)
			{
				ENGINE_LOG("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_START;
			}

			break;

		case MAIN_START:

			ENGINE_LOG("Application Start --------------");
			if (App->Start() == false)
			{
				ENGINE_LOG("Application Start exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				ENGINE_LOG("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			ENGINE_LOG("Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				ENGINE_LOG("Application CleanUp exits with error -----");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;
		}
	}

	ENGINE_LOG("Bye :)\n");
	delete engineLog;
	return main_return;
}