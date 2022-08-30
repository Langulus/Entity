#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT UI MODULE INTERFACE														
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleUI : public AModule {
		REFLECT(AModuleUI);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT UI COMPONENT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitUI : public AUnit {
		REFLECT(AUnitUI);
		using AUnit::AUnit;
	};

} // namespace PCFW

LANGULUS_DECLARE_EXCEPTION(UI);
