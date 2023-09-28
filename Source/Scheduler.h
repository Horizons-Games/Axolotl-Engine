#pragma once

class Schedulable;

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

public:
	void ScheduleTask(Schedulable&& taskToSchedule);
	void RunTasks();

private:
	std::mutex schedulerMutex;
	std::queue<Schedulable> scheduledTasks;
};
