#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT HARDWARE MODULE 															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleHardware : public AModule {
		REFLECT(AModuleHardware);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT HARDWARE UNIT 																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitHardware : public AUnit {
		REFLECT(AUnitHardware);
		using AUnit::AUnit;
	};

} // namespace PCFW
