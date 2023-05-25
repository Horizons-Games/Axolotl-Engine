#include "Application.h"
#ifdef DEBUG
	#include "optick.h"
#endif // DEBUG

enum class main_states
{
	MAIN_CREATION,
	MAIN_INIT,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

std::unique_ptr<Application> App;
#ifdef ENGINE
std::unique_ptr<EngineLog> engineLog = std::make_unique<EngineLog>();
#endif // ENGINE

int main(int argc, char** argv)
{
	int mainReturn = EXIT_FAILURE;
	main_states state = main_states::MAIN_CREATION;

	while (state != main_states::MAIN_EXIT)
	{
#ifdef DEBUG
		OPTICK_FRAME("MainThread");
#endif // DEBUG

		switch (state)
		{
			case main_states::MAIN_CREATION:

				ENGINE_LOG("Application Creation --------------");
				App = std::make_unique<Application>();
				state = main_states::MAIN_INIT;
				break;

			case main_states::MAIN_INIT:
				ENGINE_LOG("Application Init --------------");
				if (App->Init() == false)
				{
					ENGINE_LOG("Application Init exits with error -----");
					state = main_states::MAIN_EXIT;
				}
				else
				{
					state = main_states::MAIN_START;
				}

				break;

			case main_states::MAIN_START:

				ENGINE_LOG("Application Start --------------");
				if (App->Start() == false)
				{
					ENGINE_LOG("Application Start exits with error -----");
					state = main_states::MAIN_EXIT;
				}
				else
				{
					state = main_states::MAIN_UPDATE;
				}

				break;

			case main_states::MAIN_UPDATE:
			{
				update_status updateReturn = App->Update();

				if (updateReturn == update_status::UPDATE_ERROR)
				{
					ENGINE_LOG("Application Update exits with error -----");
					state = main_states::MAIN_EXIT;
				}

				if (updateReturn == update_status::UPDATE_STOP)
					state = main_states::MAIN_FINISH;
			}
			break;

			case main_states::MAIN_FINISH:

				ENGINE_LOG("Application CleanUp --------------");
				if (App->CleanUp() == false)
				{
					ENGINE_LOG("Application CleanUp exits with error -----");
				}
				else
					mainReturn = EXIT_SUCCESS;

				state = main_states::MAIN_EXIT;

				break;
		}
	}

	ENGINE_LOG("Bye :)\n");
#ifdef DEBUG
	OPTICK_SHUTDOWN();
#endif // DEBUG

	return mainReturn;
}