#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT NETWORK MODULE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleNetwork : public AModule {
		REFLECT(AModuleNetwork);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT NETWORK UNIT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitNetwork : public AUnit {
		REFLECT(AUnitNetwork);
		using AUnit::AUnit;
	};

} // namespace PCFW
