#include <vector>
#include <chrono>

namespace NRTSimulator
{

	struct TLatencyTaskParams {
		std::chrono::time_point<std::chrono::high_resolution_clock> Start;
		std::chrono::time_point<std::chrono::high_resolution_clock> End;
		int CPU;
		std::string HighLatencyScript;
	};

	class TLatencyTaskSet
	{
	private:
		std::vector<int> CPUS;
		std::chrono::time_point<std::chrono::high_resolution_clock> StartSimulation;
		std::chrono::time_point<std::chrono::high_resolution_clock> EndSimulation;
		std::vector<TLatencyTaskParams> Params;
		std::vector<pthread_t> ThreadIds;
		const std::string & HighLatencyScript;
	public:
		TLatencyTaskSet(const std::vector <int> cpus,
		                const std::string & highLatencyScript);
		void Run(std::chrono::time_point<std::chrono::high_resolution_clock> start,
		         std::chrono::time_point<std::chrono::high_resolution_clock> end);
		void Join();
	};
}
