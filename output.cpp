#include <fstream>

#include "output.h"
namespace NRTSimulator
{
	TOutput::TOutput(const std::string & directory)
		: Directory(directory)
	{

	}
	void TOutput::Write(const std::vector<std::shared_ptr<TTask>> & tasks) const
	{
		for (const auto & task : tasks) {
			std::ofstream file(Directory + "/" + task->GetName());
			file << "ResponceTime" << std::endl;
			std::vector<long long> responceTimes = task->GetResponceTimes();

			for (const auto & responceTime : responceTimes) {
				file << responceTime << std::endl;
			}

			file.close();
		}
	}
}
