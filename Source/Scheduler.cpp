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
	std::scoped_lock(schedulerMutex);
	scheduledTasks.push(std::move(taskToSchedule));
}

void Scheduler::RunTasks()
{
	std::scoped_lock(schedulerMutex);
	while (!scheduledTasks.empty())
	{
		const Schedulable& task = scheduledTasks.front();
		task();
		scheduledTasks.pop();
	}
}
