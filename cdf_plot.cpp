#include "cdf_plot.h"

namespace NRTSimulator {
	const double TCDFPlot::SCALE = 1000000.0;
	void TCDFPlot::Plot(const std::vector<long long> & points, const std::string & name) {
        FILE* pipehandle = popen("gnuplot -persist","w"); 
        fprintf(pipehandle, "set term wxt title '%s'\n", name.c_str());
        fprintf(pipehandle, "set xlabel 'ResponceTime (ms)'\n");
        fprintf(pipehandle, "set ylabel 'Probability'\n");
        fprintf(pipehandle, "plot \"-\" u 1:(1./%lu) smooth cumulative title 'CDF'\n", points.size());        
        for (const auto & point : points) {
            std::fprintf(pipehandle, "%f\n", point / SCALE);
        }
        fprintf(pipehandle, "e\n");
        fprintf(pipehandle,"quit\n");
        fflush(pipehandle);
        fclose(pipehandle);
	}
}