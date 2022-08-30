#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT ORGANICS MODULE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleOrganics : public AModule {
		REFLECT(AModuleOrganics);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT ORGANICS UNIT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitOrganics : public AUnit {
		REFLECT(AUnitOrganics);
		using AUnit::AUnit;
	};

} // namespace PCFW
