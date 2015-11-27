#pragma once

#include <vector>
#include <algorithm>
#include <chrono>

#include <random>


using std::vector;
using std::pair;

namespace NRTSimulator {
	class TRandomVar {
	private:
		std::discrete_distribution<> Distribution;
		vector<long long> Values;
    	std::minstd_rand Generator;
	public:		
		TRandomVar(const std::vector<double> & mass, const vector<long long> & values);
		long long Sample();
	};
}
