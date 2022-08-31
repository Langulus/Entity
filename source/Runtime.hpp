#pragma once
#include "Module.hpp"
#include "Unit.hpp"

namespace Langulus::Entity
{

	class AFile;
	class AFolder;


	///																								
	///	RUNTIME COMPONENT																		
	///																								
	/// Handles loading and unloading of modules, dispatches verbs to modules	
	/// Handles overall system state and console input									
	///																								
	class Runtime {
	public:
		Runtime();
		Runtime(Runtime&&) noexcept = default;
		~Runtime();

	public:
		SharedLibrary LoadPCLIB(const Path&);
		void UnloadPCLIB(SharedLibrary);

		void InstantiateModule(const Construct&, TAny<AModule*>&);

		NOD() Ptr<AModule> GetModule(DMeta);
		NOD() Ptr<const AModule> GetModule(DMeta) const;
		NOD() Ptr<AModule> GetModule(const Text&);
		NOD() Ptr<const AModule> GetModule(const Text&) const;

		void Update(Time);

		NOD() Count GetNumberOfActiveWindows() const;
		NOD() AFile* GetFile(const Path&);
		NOD() AFolder* GetFolder(const Path&);

	private:
		NOD() Ptr<AModule> CreateModule(DMeta);

	private:
		// Loaded modules, sorted by priority										
		TMap<Real, TAny<AModule*>> mModules;
		// Double protection for loaded libraries									
		TMap<Text, SharedLibrary> mLibraries;
	};

} // namespace Langulus::Entity
