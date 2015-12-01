#include "tasks_file_parser.h"
#include <iostream>

namespace NRTSimulator {
	std::vector<TTaskSpec> TTaskFileParser::Parse(std::istream & file) {
		std::vector<TTaskSpec> result;

		size_t numberOfTasks;
		file >> numberOfTasks;

		for (size_t taskNumber = 0; taskNumber < numberOfTasks; ++taskNumber) {
			TTaskSpec taskSpec;
			std::getline(file, taskSpec.Name);
			std::getline(file, taskSpec.Name);
			file >> taskSpec.CPU;
			file >> taskSpec.Priority;
			file >> taskSpec.Period;			
			size_t numberOfMasses;
			file >> numberOfMasses;

			std::pair<double, long long> massExecutionTimePair;
			for (size_t massNumber = 0; massNumber < numberOfMasses; ++massNumber) {
				file >> massExecutionTimePair.first;
				file >> massExecutionTimePair.second;

				taskSpec.ExecutionTimeSpec.push_back(massExecutionTimePair);
			}

			result.push_back(taskSpec);
		}

		return result;
	}
}