#pragma once
#include "Resolvable.hpp"

namespace Langulus::Entity
{

	/// Predeclarations																			
	class AModule;
	class CRuntime;
	struct ModuleInfo;

	/// Library handle																			
	using SharedLibrary = intptr_t;

	/// Entry and exit templates																
	/// The module entry point receives the current system and a handle to		
	/// the shared library																		
	using ModuleLinkPointPtr = void (*)();
	using ModuleCreatePointPtr = AModule* (*)(CRuntime*, SharedLibrary);
	using ModuleInfoPointPtr = const ModuleInfo& (*)();
	using ModuleExitPointPtr = void (*)();


	///																								
	///	ABSTRACT MODULE																		
	///																								
	class Module : public Resolvable {
	public:
		Module(DMeta, CRuntime*, SharedLibrary) noexcept;

		/// Never allow construction of uninitialized modules							
		Module() = delete;
		Module(const Module&) = delete;
		Module(Module&&) = delete;
		
		Module& operator = (const Module&) noexcept = default;
		Module& operator = (Module&&) noexcept = default;

	public:
		virtual void Update(Time) = 0;

	public:
		NOD() static SharedLibrary LOAD(
			const Path&, 
			ModuleLinkPointPtr&, 
			ModuleInfoPointPtr&, 
			ModuleExitPointPtr&
		);

		NOD() static Ptr<Module> CREATE(const Ptr<CRuntime>&, SharedLibrary);
		static void UNLOAD(SharedLibrary);
		NOD() static const ModuleInfo& INFO(SharedLibrary);
		NOD() SharedLibrary GetHandle() const noexcept;

	private:
		// Reference to the actual module, as loaded by the OS				
		SharedLibrary	mLibraryHandle = 0;
		// Runtime that owns the module instance									
		Own<CRuntime*> mRuntime;
	};


	///																								
	///	Module information																	
	///																								
	struct ModuleInfo {
		// Define the order in which module updates, relative to others	
		Real mPriority = 0;
		// Name of the module															
		Token mName;
		// Information about the module												
		Token mInfo;
		// Module file depo																
		Token mDepository;
		// Module abstract type															
		DMeta mCategory {};
	};


	/// Convenience macro for implementing module entry and exit points			
	#define LANGULUS_DEFINE_MODULE(module) \
		AModule* LANGULUS_MODULE_CREATOR (CRuntime* system, PCFW::SharedLibrary handle) {\
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
			LANGULUS_EXPORT() AModule* LANGULUS_MODULE_CREATOR (CRuntime*, PCFW::SharedLibrary);\
			LANGULUS_EXPORT() const PCFW::ModuleInfo& LANGULUS_MODULE_INFO ();\
			LANGULUS_EXPORT() void LANGULUS_MODULE_DESTROYER ();\
		}

} // namespace Langulus::Entity
