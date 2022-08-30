#include "Entity.hpp"
#include "CRuntime.hpp"
#include "../Flow/CFlow.hpp"
#include "../Modules/AModulePlatform.hpp"
#include "../Modules/AModuleVFS.hpp"
#include <iostream>
#include <string>

namespace PCFW
{

	/// Default runtime component construction											
	CRuntime::CRuntime()
		: AUnit {MetaData::Of<CRuntime>()} {
		pcLogSelfVerbose << "Initializing...";
		ClassValidate();
		pcLogSelfVerbose << "Initialized";
	}

	/// Runtime component destruction														
	CRuntime::~CRuntime() {
		while (!mLibraries.IsEmpty())
			UnloadPCLIB(mLibraries.Values()[0]);
	}

	/// Create a module instance, relying on the loaded dynamic libraries		
	///	@param type - the meta definition for the module to instantiate		
	///	@return the new module instance													
	Ptr<AModule> CRuntime::CreateModule(DMeta type) {
		// Scan loaded libraries and try producing								
		for (auto& library : mLibraries) {
			const auto& info = AModule::INFO(library);
			if (!type->InterpretsAs(info.mAbstractModule.GetMeta())) {
				// This library can't produce the request							
				continue;
			}

			auto module = AModule::CREATE(this, library);
			mModules.Merge(info.mPriority, module.Get());
			mModules.Sort(uiSmallest);
			pcLogSelfVerbose 
				<< "Module " << info.mName 
				<< " registered with priority " << info.mPriority;
			return module;
		}

		// If this is reached, then module wasn't produced						
		return nullptr;
	}

	/// Create a module instance or return an already instantiated one			
	///	@param construct - module initialization construct							
	///	@param products - [out] the resulting module instances					
	void CRuntime::InstantiateModule(const Construct& construct, TAny<AModule*>& products) {
		const auto type = construct.GetMeta();
		auto module = GetModule(type);
		if (!module) {
			// A module instance doesn't exist yet, so instantiate			
			pcLogSelfWarning
				<< "Required module " << type->GetToken()
				<< " is not instanced - attempting to create module...";

			module = CreateModule(type);

			if (!module) {
				throw Except::BadConstruction(pcLogSelfError
					<< "Can't instantiate module " << type->GetToken()
					<< " - most likely due to bad initialization, or a missing DLL/SO file");
			}
		}

		// Delegate any configuration to the module instance					
		Any moduleBlock { module->GetBlock() };
		UNUSED() Any unusedSideproducts;
		Verb::DefaultCreateInner(moduleBlock, construct.GetAll(), unusedSideproducts);
		products << module;
	}

	/// Load a DLL/SO extension module														
	///	@param filename - the file for the dynamic library							
	///	@return the module handle (OS dependent)										
	PCLIB CRuntime::LoadPCLIB(const Path& filename) {
		// Clone into our memory, just in case.									
		auto filename_ours = filename.Clone();

		// Check if this library is already loaded								
		const auto preloaded_name = mLibraries.FindKey(filename_ours);
		if (preloaded_name != uiNone) {
			// Never load libraries more than once									
			return mLibraries.GetValue(preloaded_name);
		}

		// Load and instantiate the module											
		ModuleLinkPointPtr linker;
		ModuleInfoPointPtr infop;
		ModuleExitPointPtr exitp;
		const auto library = AModule::LOAD(filename, linker, infop, exitp);
		if (0 == library)
			return 0;

		try {
			// Link the module - this shall register meta definitions		
			// It might throw if out of memory or if meta collision			
			linker();
		}
		catch (const AException& exception) {
			// Probably meta data conflict											
			pcLogSelfError << "Could not load: " << filename;
			pcLogSelfError << "Exception: " << exception;
			exitp();
			AModule::UNLOAD(library);
			throw;
		}

		// Get info and push to pending modules									
		mLibraries.Add(filename_ours, library);
		return library;
	}

	/// Unload a DLL/SO extension module													
	///	@param module - the module handle to unload									
	void CRuntime::UnloadPCLIB(PCLIB moduleHandle) {
		// Remove every pointer to this module										
		for (auto& group : mModules) {
			for (auto& module : group) {
				if (module->GetHandle() != moduleHandle)
					continue;
				group.Remove(module);
				//TODO check if deleted!
				break;
			}
		}

		// Remove every module occuring the module registry					
		AModule::UNLOAD(moduleHandle);
		mLibraries.RemoveValue(moduleHandle);
	}

	/// Get a DLL/SO extension module by data type										
	///	@param type - the module meta to get											
	///	@return the module instance														
	Ptr<AModule> CRuntime::GetModule(DMeta type) {
		for (auto& module_group : mModules) {
			for (auto& mod : module_group) {
				if (mod->ClassMeta()->InterpretsAs(type))
					return mod;
			}
		}

		return nullptr;
	}

	/// Get a DLL/SO extension module by data type (const)							
	///	@param type - the module meta to get											
	///	@return the module instance														
	Ptr<const AModule> CRuntime::GetModule(DMeta type) const {
		return const_cast<CRuntime*>(this)->GetModule(type);
	}

	/// Get a DLL/SO extension module by token name										
	///	@param token - module name token													
	///	@return the module instance, or nullptr if not found						
	Ptr<AModule> CRuntime::GetModule(const Text& token) {
		for (auto& pair : mModules) {
			for (auto& mod : pair) {
				if (LiteralText{ token } == mod->ClassMeta()->GetToken())
					return mod;
			}
		}

		return nullptr;
	}

	/// Get a DLL/SO extension module by token name										
	///	@param mname - module name token													
	///	@return the module instance, or nullptr if not found						
	Ptr<const AModule> CRuntime::GetModule(const Text& token) const {
		return const_cast<CRuntime*>(this)->GetModule(token);
	}

	/// Get number of active windows															
	///	@return the number of active windows											
	pcptr CRuntime::GetNumberOfActiveWindows() const {
		const auto platform = GetModule(DataID::Reflect<AModulePlatform>());
		if (platform && platform->IsClassValid())
			return platform.As<AModulePlatform>()->GetNumberOfActiveWindows();
		return 0;
	}

	/// Get a file interface (loads MVFS if not loaded yet)							
	///	@param filename - the file to get												
	///	@return the file interface															
	AFile* CRuntime::GetFile(const Path& filename) {
		auto fs = GetModule(DataID::Reflect<AModuleVFS>());
		if (!fs)
			fs = CreateModule(DataID::Reflect<AModuleVFS>());
		return fs.As<AModuleVFS>()->GetFile(filename);
	}

	/// Get a folder interface (loads MVFS if not loaded yet)						
	///	@param dirname - the directory to get											
	///	@return the folder interface														
	AFolder* CRuntime::GetFolder(const Path& dirname) {
		auto fs = GetModule(DataID::Reflect<AModuleVFS>());
		if (!fs)
			fs = CreateModule(DataID::Reflect<AModuleVFS>());
		return fs.As<AModuleVFS>()->GetFolder(dirname);
	}

	/// Update the runtime, by updating each active module							
	///	@param dt - delta time between update calls									
	void CRuntime::Update(PCTime dt) {
		// Update all modules															
		for (auto& pair : mModules) {
			for (auto& mod : pair) {
				if (mod->IsClassValid())
					mod->Update(dt);
			}
		}

		// If you close all interactive windows, or none are available,	
		// you're returned to console mode											
		if (GetNumberOfActiveWindows())
			return;

		// No active windows at this point											
		// We're in console mode, so collect commands forever, until		
		// a shutdown command, or closing the console							
		for (auto owner : GetOwners()) {
			pcLogChoice << owner << ": Waiting for command:\n";
			std::wstring line;
			std::getline(std::wcin, line);
			try {
				owner->DoGASM(GASM(line.c_str()));
			}
			catch (...) {

			}
		}
	}

} // namespace PCFW
