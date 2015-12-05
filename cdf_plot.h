#include <vector>
#include <string>

namespace NRTSimulator
{
	class TCDFPlot
	{
	private:
		static const double SCALE;
	public:
		void Plot(const std::vector<long long> & points, const std::string & name);
	};
}