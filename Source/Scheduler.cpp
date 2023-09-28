#include "StdAfx.h"

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
	if (!taskToSchedule)
	{
		LOG_WARNING("Trying to schedule an empty task!");
		return;
	}
	std::scoped_lock(schedulerMutex);
	scheduledTasks.push(std::move(taskToSchedule));
}

void Scheduler::RunTasks()
{
	std::scoped_lock(schedulerMutex);
	while (!scheduledTasks.empty())
	{
		const Schedulable& task = scheduledTasks.front();
		if (task)
		{
			task();
		}
		else
		{
			// Error, because ScheduleTask should prevent this
			// If this becomes too frequent we could index/ID the tasks and also log that index
			// So we can see who schedules it
			LOG_ERROR("Trying to run an empty task!");
		}
		scheduledTasks.pop();
	}
}
