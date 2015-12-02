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
		/*
		* Number_Of_Tasks
		* Task_1_Name
		* Task_1_CPU
		* Task_1_Priority
		* Task_1_Perion		
		* Task_1_Number_Of_Execution_Time_masses
		* Task_1_Mass_1 Task_1_Execution_Time_1
		* ...
		* Taks_2_Name
		* ...
		*/
		std::vector<std::shared_ptr<TTask>> Parse(std::istream &);
	};
}