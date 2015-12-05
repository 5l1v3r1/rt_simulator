#include <string>
#include <memory>
#include <vector>

#include "task.h"
namespace NRTSimulator
{
	class TOutput
	{
	private:
		std::string Directory;
	public:
		TOutput(const std::string & directory);
		void Write(const std::vector<std::shared_ptr<TTask>> & tasks) const;
	};
}