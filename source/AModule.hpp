#pragma once
#include "Resolvable.hpp"

namespace Langulus::Entity
{

	/// Predeclarations																			
	class AModule;
	class CRuntime;
	struct ModuleInfo;

	/// Library handle																			
	using PCLIB = intptr_t;

	/// Entry and exit templates																
	/// The module entry point receives the current system and a handle to		
	/// the shared library																		
	using ModuleLinkPointPtr = void (*)();
	using ModuleCreatePointPtr = AModule* (*)(CRuntime*, PCLIB);
	using ModuleInfoPointPtr = const ModuleInfo& (*)();
	using ModuleExitPointPtr = void (*)();


	///																								
	///	ABSTRACT MODULE																		
	///																								
	class AModule : public Resolvable {
	public:
		AModule(DMeta, CRuntime*, PCLIB) noexcept;

		/// Never allow construction of uninitialized modules							
		AModule() = delete;
		AModule(const AModule&) = delete;
		AModule(AModule&&) = delete;
		
		AModule& operator = (const AModule&) noexcept = default;
		AModule& operator = (AModule&&) noexcept = default;

	public:
		virtual void Update(PCTime) = 0;

	public:
		NOD() static PCLIB LOAD(
			const Path&, 
			ModuleLinkPointPtr&, 
			ModuleInfoPointPtr&, 
			ModuleExitPointPtr&
		);

		NOD() static Ptr<AModule> CREATE(const Ptr<CRuntime>&, PCLIB);
		static void UNLOAD(PCLIB);
		NOD() static const ModuleInfo& INFO(PCLIB);
		NOD() PCLIB GetHandle() const noexcept;

	private:
		// Reference to the actual module, as loaded by the OS				
		PCLIB	mLibraryHandle = 0;
		// Runtime that owns the module instance									
		Own<CRuntime*> mRuntime;
	};


	///																								
	///	Module information																	
	///																								
	struct ModuleInfo {
		// Priority of the module														
		pcr32 mPriority = 0;
		// Name of the module															
		LiteralText mName;
		// Information about the module												
		LiteralText mInfo;
		// Module file depo																
		LiteralText mDepository;
		// Module abstract type															
		DataID mAbstractModule = DataID::Of<AModule>;
	};


	/// Convenience macro for implementing module entry and exit points			
	#define LANGULUS_DEFINE_MODULE(module) \
		AModule* LANGULUS_MODULE_CREATOR (CRuntime* system, PCFW::PCLIB handle) {\
			static_assert(!pcIsAbstract<module>, "Langulus module " #module " is abstract, have you forgotten to define its interface?"); \
			return new module(system, handle);\
		}\
		const PCFW::ModuleInfo& LANGULUS_MODULE_INFO () {\
			return gsModuleInfo;\
		}

	/// Define module entry and exit points, as well as module info				
	///	@param prio - priority of the module											
	///	@param name - name of the module													
	///	@param info - verbose info about the module's purpose						
	///	@param depo - default resource import/export directory					
	///	@param abst - abstract module type												
	#define LANGULUS_DECLARE_MODULE(prio, name, info, depo, abst) \
		const PCFW::ModuleInfo gsModuleInfo = {prio, name, info, depo, DataID::Of<abst>};\
		extern "C" {\
			LANGULUS_EXPORT() void LANGULUS_MODULE_LINKER ();\
			LANGULUS_EXPORT() AModule* LANGULUS_MODULE_CREATOR (CRuntime*, PCFW::PCLIB);\
			LANGULUS_EXPORT() const PCFW::ModuleInfo& LANGULUS_MODULE_INFO ();\
			LANGULUS_EXPORT() void LANGULUS_MODULE_DESTROYER ();\
		}

} // namespace Langulus::Entity
