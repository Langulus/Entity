#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT ECONOMY MODULE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleEconomy : public AModule {
		REFLECT(AModuleEconomy);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT ECONOMY UNIT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitEconomy : public AUnit {
		REFLECT(AUnitEconomy);
		using AUnit::AUnit;
	};

} // namespace PCFW
