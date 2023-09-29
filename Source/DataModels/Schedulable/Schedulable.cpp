#include "StdAfx.h"

#include "DataModels/Schedulable/Schedulable.h"

#include "FileSystem/UIDGenerator.h"

Schedulable::Schedulable(std::function<void(void)>&& task, std::uint16_t frameDelay) :
	schedulableId(UniqueID::GenerateUID()),
	task(std::move(task)),
	frameDelay(frameDelay)
{
}

Schedulable::~Schedulable() = default;

SchedulableRunResult Schedulable::Run()
{
	if (0U < frameDelay)
	{
		--frameDelay;
		return SchedulableRunResult::DELAYED;
	}
	if (!task)
	{
		LOG_WARNING("Schedulable {} with an empty task was attempted to run", GetId());
		return SchedulableRunResult::TASK_WAS_EMPTY;
	}
	task();
	return SchedulableRunResult::SUCCESS;
}
