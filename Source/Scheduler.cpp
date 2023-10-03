#include "StdAfx.h"

#include "DataModels/Schedulable/Schedulable.h"

#include "Scheduler.h"

 Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
	// clear the queue without iterating it
	std::queue<Schedulable>().swap(scheduledTasks);
}

void Scheduler::ScheduleTask(Schedulable&& taskToSchedule)
{
	std::scoped_lock(schedulerMutex);
	scheduledTasks.push(std::move(taskToSchedule));
}

void Scheduler::RunTasks()
{
	std::scoped_lock(schedulerMutex);
	std::queue<Schedulable> remainingTasks;
	while (!scheduledTasks.empty())
	{
		Schedulable& schedulable = scheduledTasks.front();
		SchedulableRunResult result = schedulable.Run();
		if (result == SchedulableRunResult::DELAYED)
		{
			remainingTasks.push(std::move(schedulable));
		}
		else if (result == SchedulableRunResult::TASK_WAS_EMPTY)
		{
			LOG_ERROR("Trying to run empty schedulable {}!", schedulable.GetId());
		}
		scheduledTasks.pop();
	}
	scheduledTasks = std::move(remainingTasks);
}
