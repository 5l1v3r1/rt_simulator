#include <numeric>
#include <algorithm>
#include <random>

#include <sys/types.h>
#include <unistd.h>

#include "random_var.h"

namespace NRTSimulator {
	TRandomVar::TRandomVar(const std::vector<double> & mass, const vector<long long> & values)
		: Distribution(mass.begin(), mass.end())
		, Values(values)
		, Generator(getpid())
	{
	}

	long long TRandomVar::Sample() {
		return Values[Distribution(Generator)];
	}

	long long TRandomVar::GetMaxValue() const {
		return *std::max_element(Values.begin(), Values.end());
	}	
}
