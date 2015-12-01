#pragma once
#include <vector>
#include <string>
#include <utility>
#include <istream>

namespace NRTSimulator {
	struct TTaskSpec {
		std::vector<std::pair<double, long long>> ExecutionTimeSpec;
		long long Period;
		int CPU;
		int Priority;
		std::string Name;		
	};

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
		std::vector<TTaskSpec> Parse(std::istream &);
	};
}