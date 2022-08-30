#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"

namespace PCFW
{

	///																								
	///	ABSTRACT AI MODULE																	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleAI : public AModule {
		REFLECT(AModuleAI);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT AI UNIT																		
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitAI : public AUnit {
		REFLECT(AUnitAI);
		using AUnit::AUnit;

		virtual void DoAIAD(const Text&, CFlow&) = 0;
	};

} // namespace PCFW

LANGULUS_DECLARE_EXCEPTION(AI);
