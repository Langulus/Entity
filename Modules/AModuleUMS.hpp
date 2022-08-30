#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT USER MANAGEMENT MODULE													
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleUMS : public AModule {
		REFLECT(AModuleUMS);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT USER MANAGEMENT UNIT														
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitUMS : public AUnit {
		REFLECT(AUnitUMS);
		using AUnit::AUnit;
	};

} // namespace PCFW
