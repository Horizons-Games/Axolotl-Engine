#include "StdAfx.h"

#include "DataModels/Schedulable/Schedulable.h"

#include "Scheduler.h"

Scheduler::Scheduler() = default;

Scheduler::~Scheduler()
{
	// clear the queue without iterating it
	std::queue<Schedulable>().swap(scheduledTasks);
}

void Scheduler::ScheduleTask(Schedulable&& taskToSchedule)
{
	std::unique_lock lock(schedulerMutex);
	scheduledTasks.push(std::move(taskToSchedule));
}

void Scheduler::RunTasks()
{
	// snapshot the state of the queue at the beginning of the method,
	// to prevent it from being modified while the loop is executing
	std::queue<Schedulable> scheduledTasksSnapshot;
	{
		std::unique_lock lock(schedulerMutex);
		scheduledTasksSnapshot = std::move(scheduledTasks);
	}

	std::queue<Schedulable> remainingTasks;
	while (!scheduledTasksSnapshot.empty())
	{
		Schedulable& schedulable = scheduledTasksSnapshot.front();
		SchedulableRunResult result = schedulable.Run();
		if (result == SchedulableRunResult::DELAYED)
		{
			remainingTasks.push(std::move(schedulable));
		}
		else if (result == SchedulableRunResult::TASK_WAS_EMPTY)
		{
			LOG_ERROR("Trying to run empty schedulable {}!", schedulable.GetId());
		}
		scheduledTasksSnapshot.pop();
	}

	// now push all remaining elements into the back of the queue
	{
		std::unique_lock lock(schedulerMutex);
		while (!remainingTasks.empty())
		{
			scheduledTasks.push(remainingTasks.front());
			remainingTasks.pop();
		}
	}
}
