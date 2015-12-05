#include <getopt.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

#include "cmd_arg_parser.h"

namespace NRTSimulator
{
	const std::string TCmdArgParser::USAGE =
	    "Usage: %s [-f file] [-o directory] [-s number] [-plc]\n";
	const std::string TCmdArgParser::HELP =
	    USAGE +
	    "Simulate set of real time tasks, and perform responce time analysis\n"
	    "  -f,--file\t\tfile with task parameter (default task_spec.cfg)\n"
	    "  -o,--output\t\tdirectory to which put simulated responce times of the\n\t\t\t tasks (default no output)\n"
	    "  -s,--simtime\t\ttime of simulation in seconds (default 10s)\n"
	    "  -p,--plot\t\tplot task CDF (usign gnuplot)\n"
	    "  -k,--latency\t\trun tasks with hight kernel latency\n\t\t\t and estimate kernel latency\n"
	    "  -c,--counting\t\tuse counting to model execution time\n\t\t\t (instead of busy wait on timer)\n";

	TCmdArgParser::TCmdArgParser(int argc, char *argv[])
		: TaskSpecFileName("task_spec.cfg")
		, OutputDirecory("")
		, SimulationTime(10)
		, PlotNeeded(false)
		, HighKernelLatencyNeeded(false)
		, CountingUsed(false)
	{
		char const* programName = argv[0];

		static struct option long_options[] = {
			{"file",    required_argument, 0, 'f'},
			{"simtime", required_argument, 0, 's'},
			{"plot", no_argument, 0, 'p'},
			{"counting", no_argument, 0, 'c'},
			{"output", required_argument, 0, 'o'},
			{"latency", no_argument, 0, 'l'},
			{"help", no_argument, 0, 'h'},
			{0, 0, 0,  0}
		};

		struct stat st = {0};

		int long_index = 0;
		int opt = 0;

		while ((opt = getopt_long(argc, argv, "hplcf:s:o:", long_options,
		                          &long_index)) != -1) {
			switch (opt) {
			case 'f' :
				TaskSpecFileName = std::string(optarg);
				break;

			case 's' :
				SimulationTime = std::stoll(optarg);
				break;

			case 'c':
				CountingUsed = true;
				break;

			case 'p':
				PlotNeeded = true;
				break;

			case 'l':
				HighKernelLatencyNeeded = true;
				break;

			case 'o':
				OutputDirecory = std::string(optarg);

				if (stat(OutputDirecory.c_str(), &st) == -1) {
					if (mkdir(OutputDirecory.c_str(), 0777) < 0) {
						std::cout << "Failed to create output directory. Wrong --output(-o) parameter."
						          << std::endl;
						exit(-1);
					}
				}

				break;

			case 'h':
				printf(HELP.c_str(), programName);
				exit(0);

			default:
				printf(USAGE.c_str(), programName);
				exit(-1);
			}
		}
	}
	const std::string & TCmdArgParser::GetTaskSpecFileName() const
	{
		return TaskSpecFileName;
	}
	const std::string & TCmdArgParser::GetOutputDirectory() const
	{
		return OutputDirecory;
	}
	long long TCmdArgParser::GetSimulationTime() const
	{
		return SimulationTime;
	}
	bool TCmdArgParser::IsPlotNeeded() const
	{
		return PlotNeeded;
	}
	bool TCmdArgParser::IsHighKernelLatencyNeeded() const
	{
		return HighKernelLatencyNeeded;
	}
	bool TCmdArgParser::IsCountingUsed() const
	{
		return CountingUsed;
	}
}