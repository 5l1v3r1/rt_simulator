#include <string>

namespace NRTSimulator {
	class TCmdArgParser {
	private:
		static const std::string USAGE;
		static const std::string HELP;
		std::string TaskSpecFileName;
		std::string OutputDirecory;
		long long SimulationTime;
		bool PlotNeeded;
		bool HighKernelLatencyNeeded;
		bool CountingUsed;
	public:
		TCmdArgParser(int argc, char *argv[]);
		const std::string & GetTaskSpecFileName() const;
		const std::string & GetOutputDirectory() const;
		long long GetSimulationTime() const;
		bool IsPlotNeeded() const;
		bool IsHighKernelLatencyNeeded() const;
		bool IsCountingUsed() const ;
	};
}