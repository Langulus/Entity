#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT BIG DATA MODULE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleData : public AModule {
		REFLECT(AModuleData);
	public:
		using AModule::AModule;
	};

} // namespace PCFW