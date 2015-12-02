#include "tasks_file_parser.h"


#include <iostream>


namespace NRTSimulator {
	std::vector<std::shared_ptr<TTask>> TTaskFileParser::Parse(std::istream & file) {

		//TODO: add error handling
		std::vector<std::shared_ptr<TTask>> result;
		int cpu, priority;
		std::string name;
		long long period;

		std::vector<double> probMasses;
		std::vector<long long> probTimes;
		size_t numberOfTasks;

		file >> numberOfTasks;

		for (size_t taskNumber = 0; taskNumber < numberOfTasks; ++taskNumber) {
			std::getline(file, name);
			std::getline(file, name);
			file >> cpu;
			file >> priority;
			file >> period;

			size_t numberOfMasses;
			file >> numberOfMasses;
			probMasses.clear();
			probTimes.clear();
			double probMass;
			long long probTime;
			for (size_t massNumber = 0; massNumber < numberOfMasses; ++massNumber) {
				file >> probMass;
				file >> probTime;

				probMasses.push_back(probMass);
				probTimes.push_back(probTime);				
			}

			result.push_back(std::shared_ptr<TTask>
				(new TTimerTask(TRandomVar(probMasses, probTimes), period, cpu, priority, name)));
		}

		return result;
	}
}