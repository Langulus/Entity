#include "include/PCFW.hpp"

PC_MONOPOLIZE_MEMORY(0)

namespace PCFW
{

	/// Initialize the global objects required for the piception functioning	
	///	@param memory_budget - set an upper limit to dynamic memory use		
	///	@returns true if everything is ok												
	bool pcFrameworkInit(pcptr memoryBudget) {
		static bool guard = false;
		if (guard)
			return true;
		guard = true;

		Logger::pcInitLogger();
		Memory::pcInitMemory(memoryBudget);
		Flow::pcInitGASM();
		Math::pcInitPCMath();
		pcInitFrameworkMeta();
		pcLogVerbose << ccGreen << "PCFW initialized\n\n";
		return true;
	}

} // namespace PCFW
