#pragma once

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

public:
	using Schedulable = std::function<void(void)>;

	void ScheduleTask(Schedulable&& taskToSchedule);
	void RunTasks();

private:
	std::mutex schedulerMutex;
	std::queue<Schedulable> scheduledTasks;
};
