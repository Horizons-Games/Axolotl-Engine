#pragma once

#include "FileSystem/UID.h"

enum class SchedulableRunResult
{
	SUCCESS,
	DELAYED,
	TASK_WAS_EMPTY
};

class Schedulable
{
public:
	Schedulable(std::function<void(void)>&& task, std::uint16_t frameDelay);
	~Schedulable();

	SchedulableRunResult Run();

	UID GetId() const;

private:
	UID schedulableId;
	std::function<void(void)> task;
	std::uint16_t frameDelay;
};

inline UID Schedulable::GetId() const
{
	return schedulableId;
}
