#pragma once
#include <vector>
#include <string>
#include <utility>
#include <istream>
#include <memory>

#include "task.h"

namespace NRTSimulator {
	class TTaskFileParser {
	public:
		std::vector<std::shared_ptr<TTask>> Parse(const std::string &);
	};
}