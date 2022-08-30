#pragma once
#include "../Hierarchy/AModule.hpp"
#include "../Hierarchy/AUnit.hpp"

namespace PCFW
{

	///																								
	///	ABSTRACT MATH MODULE																	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleMath : public AModule {
		REFLECT(AModuleMath);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT MATH UNIT																	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitMath : public AUnit {
		REFLECT(AUnitMath);
		using AUnit::AUnit;
	};

} // namespace PCFW
