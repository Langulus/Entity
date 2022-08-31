#pragma once
#include "Module.hpp"
#include "Unit.hpp"

namespace Langulus::Entity
{

	class AFile;
	class AFolder;


	///																								
	///	Runtime																					
	///																								
	///	Handles loading and unloading of modules, dispatches verbs to			
	/// modules, handles overall system state and console input. Overall, it	
	/// takes care of the minimally required OS aspect of running a program.	
	///	You can view a Runtime as a single process inside Langulus. You can	
	/// create infinite nested subprocesses in any Entity. Each new runtime		
	/// will act as the environment for that Entity, as well as all of its		
	/// children, modules, units, etc.														
	///																								
	class Runtime final {
	public:
		Runtime();
		Runtime(Runtime&&) noexcept = default;
		~Runtime();

	public:
		SharedLibrary LoadSharedLibrary(const Path&);
		void UnloadSharedLibrary(SharedLibrary);

		Module* InstantiateModule(const Construct&);

		NOD() Module* GetModule(DMeta);
		NOD() const Module* GetModule(DMeta) const;
		NOD() Module* GetModule(const Token&);
		NOD() const Module* GetModule(const Token&) const;

		void Update(Time);

		NOD() Count GetNumberOfActiveWindows() const;
		NOD() AFile* GetFile(const Path&);
		NOD() AFolder* GetFolder(const Path&);

	private:
		// Loaded shared libraries, indexed by filename							
		// This is a static registry - all Runtimes use the same shared	
		// library objects, but manage their own module instantiations		
		static THashMap<Path, SharedLibrary> mLibraries;

		// Instantiated modules, sorted by priority								
		TMap<Real, TAny<Module*>> mModules;
		// Instantiated modules, indexed by library handles					
		THashMap<SharedLibrary, TAny<Module*>> mInstantiations;
		// Instantiated modules, indexed by concrete type						
		THashMap<DMeta, TAny<Module*>> mConcreteModules;
		// Instantiated modules, indexed by category								
		THashMap<DMeta, TAny<Module*>> mCategories;
	};

} // namespace Langulus::Entity
