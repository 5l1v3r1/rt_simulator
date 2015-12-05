#pragma once
#include <vector>
#include <string>
#include <utility>
#include <istream>
#include <memory>
#include <libconfig.h++>

#include "task.h"

namespace NRTSimulator
{
	class TTaskFileParser
	{
	private:
		bool IsTimerTasks;
	public:
		TTaskFileParser(bool isTimerTasks);
		std::vector<std::shared_ptr<TTask>> Parse(const std::string &);
	private:
		TRandomVar ParseRandomVar(const libconfig::Setting &, const std::string name);
	};
}