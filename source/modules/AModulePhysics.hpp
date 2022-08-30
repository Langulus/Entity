#pragma once
#include "../Hierarchy/AModule.hpp"	
#include "../Hierarchy/AUnit.hpp"

namespace PCFW
{

	///																								
	///	ABSTRACT PHYSICS MODULE INTERFACE												
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModulePhysics : public AModule {
		REFLECT(AModulePhysics);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT PHYSICS UNIT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitPhysics : public AUnit {
		REFLECT(AUnitPhysics);
		using AUnit::AUnit;
	};


	///																								
	///	ABSTRACT UNIVERSE INTERFACE														
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUniverse : public AUnitPhysics {
		REFLECT(AUniverse);
		using AUnitPhysics::AUnitPhysics;
	};

	template<pcptr DIMENSIONS>
	using InstanceReal = TInstance<TVec<real, DIMENSIONS>>;

	///																								
	///	ABSTRACT PHYSICAL INSTANCE INTERFACE											
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AInstance : public AUnitPhysics, public InstanceReal<3> {
		REFLECT(AInstance);
		using InstanceType = InstanceReal<3>;
		using AUnitPhysics::AUnitPhysics;

		NOD() virtual bool Cull(const LodState&) const = 0;
		NOD() virtual MatrixType GetLeveledTransformation(const LodState&) const = 0;
		NOD() virtual MemberType Distance(const AInstance*) const = 0;
		NOD() virtual bool Intersect(const AInstance*) const = 0;
		virtual void LookAt(const vec4& what) = 0;
	};

} // namespace PCFW

LANGULUS_DECLARE_EXCEPTION(Physical);
