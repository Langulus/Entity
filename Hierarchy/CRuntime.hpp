#pragma once
#include "AModule.hpp"
#include "AUnit.hpp"
#include "../Containers/TMap.hpp"

namespace PCFW
{

	class AFile;
	class AFolder;
	  
	///																								
	///	RUNTIME COMPONENT																		
	///																								
	/// Handles loading and unloading of modules, dispatches verbs to modules	
	/// Handles overall system state and console input									
	///																								
	class LANGULUS_MODULE(FRAMEWORK) CRuntime : public AUnit {
		REFLECT(CRuntime);
	public:
		CRuntime();
		CRuntime(CRuntime&&) noexcept = default;
		~CRuntime();

	public:
		PCLIB LoadPCLIB(const Path&);
		void UnloadPCLIB(PCLIB);

		void InstantiateModule(const Construct&, TAny<AModule*>&);

		NOD() Ptr<AModule> GetModule(DMeta);
		NOD() Ptr<const AModule> GetModule(DMeta) const;
		NOD() Ptr<AModule> GetModule(const Text&);
		NOD() Ptr<const AModule> GetModule(const Text&) const;

		void Update(PCTime);

		NOD() pcptr GetNumberOfActiveWindows() const;
		NOD() AFile* GetFile(const Path&);
		NOD() AFolder* GetFolder(const Path&);

	private:
		NOD() Ptr<AModule> CreateModule(DMeta);

	private:
		// Loaded modules, sorted by priority										
		TMap<real, TAny<AModule*>> mModules;
		// Double protection for loaded libraries									
		TMap<Text, PCLIB> mLibraries;
	};

} // namespace PCFW
