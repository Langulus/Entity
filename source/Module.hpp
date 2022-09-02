#pragma once
#include "Resolvable.hpp"

namespace Langulus::Entity
{

	/// Predeclarations																			

	class Module;
	class Runtime;


	///																								
	///	External module interface															
	///																								
	class Module : public Resolvable {
	public:
		Module(DMeta classid, Runtime* runtime) noexcept
			: Resolvable {classid}
			, mRuntime {runtime} {}

		/// Never allow construction of uninitialized modules							
		Module() = delete;
		Module(const Module&) = delete;
		Module(Module&&) = delete;
		
		Module& operator = (const Module&) noexcept = default;
		Module& operator = (Module&&) noexcept = default;

		struct ModuleInfo {
			// Define the order in which module updates, relative to others
			Real mPriority = 0;
			// Name of the module														
			Token mName;
			// Information about the module											
			Token mInfo;
			// Relative module dedicated file folder, under Data/Modules/	
			Token mDepository;
			// Module abstract type														
			DMeta mCategory {};
		};

		using EntryPoint = void (*)();
		using CreatePoint = Module* (*)(Runtime*, const Any&);
		using InfoPoint = const ModuleInfo& (*)();
		using ExitPoint = void (*)();

	public:
		NOD() Runtime* GetRuntime() const noexcept { return mRuntime; }
		virtual void Update(Time) = 0;

	private:
		// Runtime that owns the module instance									
		Runtime* mRuntime;
	};

	/// Library handle																			
	struct SharedLibrary {
		intptr_t mHandle {};
		Module::EntryPoint mEntry;
		Module::CreatePoint mCreator;
		Module::InfoPoint mInfo;
		Module::ExitPoint mExit;
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
