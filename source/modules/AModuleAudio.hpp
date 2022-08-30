#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"	

namespace PCFW
{

	///																								
	///	ABSTRACT AUDIO MODULE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleAudio : public AModule {
		REFLECT(AModuleAudio);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT AUDIO UNIT																	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitAudio : public AUnit {
		REFLECT(AUnitAudio);
		using AUnit::AUnit;
	};

} // namespace PCFW
