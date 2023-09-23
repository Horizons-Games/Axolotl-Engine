#include "StdAfx.h"

#include "Scheduler.h"

 Scheduler::Scheduler()
{
}

Scheduler::~Scheduler()
{
	// clear the queue without iterating it
	std::queue<std::pair<Schedulable, std::uint16_t>>().swap(scheduledTasks);
}

void Scheduler::ScheduleTask(Schedulable&& taskToSchedule, std::uint16_t frameDelay)
{
	if (!taskToSchedule)
	{
		LOG_WARNING("Trying to schedule an empty task!");
		return;
	}
	std::scoped_lock(schedulerMutex);
	scheduledTasks.push(std::make_pair(std::move(taskToSchedule), frameDelay));
}

void Scheduler::RunTasks()
{
	std::scoped_lock(schedulerMutex);
	std::queue<std::pair<Schedulable, std::uint16_t>> remainingTasks;
	while (!scheduledTasks.empty())
	{
		std::pair<Schedulable, std::uint16_t>& task = scheduledTasks.front();
		if (task.second != 0U)
		{
			--task.second;
			remainingTasks.push(std::move(task));
		}
		else
		{
			task.first();
		}
		scheduledTasks.pop();
	}
	scheduledTasks = std::move(remainingTasks);
}
