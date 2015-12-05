#include <algorithm>
#include <random>

#include "random_var.h"

namespace NRTSimulator
{
	int TRandomVar::seed = 0;
	TRandomVar::TRandomVar(const std::vector<double> & mass,
	                       const std::vector<long long> & values)
		: Distribution(mass.begin(), mass.end())
		, Values(values)
		, Generator(seed++)
	{
	}

	long long TRandomVar::Sample()
	{
		return Values[Distribution(Generator)];
	}

	long long TRandomVar::GetMaxValue() const
	{
		return *std::max_element(Values.begin(), Values.end());
	}
}
