#include "StdAfx.h"

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
std::unique_ptr<AxoLog> logContext = std::make_unique<AxoLog>();
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

				LOG_VERBOSE("Application Creation --------------");
				App = std::make_unique<Application>();
				state = main_states::MAIN_INIT;
				break;

			case main_states::MAIN_INIT:
				LOG_VERBOSE("Application Init --------------");
				if (App->Init() == false)
				{
					LOG_ERROR("Application Init exits with error -----");
					state = main_states::MAIN_EXIT;
				}
				else
				{
					state = main_states::MAIN_START;
				}

				break;

			case main_states::MAIN_START:

				LOG_VERBOSE("Application Start --------------");
				if (App->Start() == false)
				{
					LOG_ERROR("Application Start exits with error -----");
					state = main_states::MAIN_EXIT;
				}
				else
				{
					state = main_states::MAIN_UPDATE;
				}

				break;

			case main_states::MAIN_UPDATE:
			{
				UpdateStatus updateReturn = App->Update();

				if (updateReturn == UpdateStatus::UPDATE_ERROR)
				{
					LOG_ERROR("Application Update exits with error -----");
					state = main_states::MAIN_EXIT;
				}

				if (updateReturn == UpdateStatus::UPDATE_STOP)
				{
					state = main_states::MAIN_FINISH;
				}
			}
			break;

			case main_states::MAIN_FINISH:

				LOG_VERBOSE("Application CleanUp --------------");
				if (App->CleanUp() == false)
				{
					LOG_ERROR("Application CleanUp exits with error -----");
				}
				else
				{
					mainReturn = EXIT_SUCCESS;
				}

				state = main_states::MAIN_EXIT;

				break;
		}
	}

	LOG_INFO("Bye :)\n");
#ifdef DEBUG
	OPTICK_SHUTDOWN();
#endif // DEBUG

	return mainReturn;
}