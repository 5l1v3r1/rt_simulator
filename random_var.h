#pragma once

#include <vector>
#include <random>

namespace NRTSimulator {
	class TRandomVar {
	private:
		std::discrete_distribution<> Distribution;
		std::vector<long long> Values;
    	std::minstd_rand Generator;
    	static int seed; 
	public:		
		TRandomVar(const std::vector<double> & mass, const std::vector<long long> & values);
		long long Sample();
		long long GetMaxValue() const;
	};
}
