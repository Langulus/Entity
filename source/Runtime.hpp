///																									
/// Langulus::Entity																				
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include "Module.hpp"
#include "Unit.hpp"

namespace Langulus::Entity
{

	class AFile;
	class AFolder;
	using ModuleList = TAny<Module*>;


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
	protected:
		/// Library handle																		
		struct SharedLibrary {
			uintptr_t mHandle {};
			Module::EntryPoint mEntry;
			Module::CreatePoint mCreator;
			Module::InfoPoint mInfo;
			Module::ExitPoint mExit;

			/// Compare shared libraries by handle only									
			bool operator == (const SharedLibrary& rhs) const noexcept {
				return mHandle == rhs.mHandle;
			}

			/// Hashing for to being able to contain it in a map						
			Hash GetHash() const noexcept {
				return {mHandle};
			}
		};

		Entity* mOwner;

		// Loaded shared libraries, indexed by filename							
		// This is a static registry - all Runtimes use the same shared	
		// library objects, but manage their own module instantiations		
		static TUnorderedMap<Path, SharedLibrary> mLibraries;

		// Shared library dependencies for all externally registered types
		static TUnorderedMap<DMeta, SharedLibrary> mDependencies;

		// Instantiated modules, sorted by priority								
		TMap<Real, ModuleList> mModules;

		// Instantiated modules, indexed by library handles					
		TUnorderedMap<SharedLibrary, ModuleList> mInstantiations;

		// Instantiated modules, indexed by type									
		TUnorderedMap<DMeta, ModuleList> mModulesByType;

		NOD() SharedLibrary LoadSharedLibrary(const Path&);
		void UnloadSharedLibrary(const SharedLibrary&);

	public:
		Runtime() = delete;
		Runtime(Entity*) noexcept;
		Runtime(Runtime&&) noexcept = default;
		~Runtime();

		NOD() Entity* GetOwner() const noexcept { return mOwner; }

	public:
		NOD() Module* InstantiateModule(const Construct&);

		NOD() const ModuleList& GetModules(DMeta) const noexcept;
		NOD() const ModuleList& GetModules(const Token&) const noexcept;

		void Update(Time);
	};

} // namespace Langulus::Entity
