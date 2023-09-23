#pragma once

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

public:
	using Schedulable = std::function<void(void)>;

	void ScheduleTask(Schedulable&& taskToSchedule, std::uint16_t frameDelay);
	void RunTasks();

private:
	std::mutex schedulerMutex;
	std::queue<std::pair<Schedulable, std::uint16_t>> scheduledTasks;
};
