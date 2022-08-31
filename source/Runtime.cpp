#include "Entity.hpp"
#include "Runtime.hpp"

#define LANGULUS_OS(a) LANGULUS_OS_##a()

#if defined(_WIN32) || defined(__CYGWIN__)
	#include <Windows.h>
	#define LANGULUS_OS_WINDOWS() 1
#else 
	#define LANGULUS_OS_WINDOWS() 0
#endif

#if defined(__linux__)
	#define LANGULUS_OS_LINUX() 1
#else 
	#define LANGULUS_OS_LINUX() 0
#endif

#if defined(__ANDROID__)
	#define LANGULUS_OS_ANDROID() 1
#else 
	#define LANGULUS_OS_ANDROID() 0
#endif

#if defined(__APPLE__)
	#define LANGULUS_OS_MACOS() 1
#else 
	#define LANGULUS_OS_MACOS() 0
#endif

#if defined(__unix__)
	#define LANGULUS_OS_UNIX() 1
#else 
	#define LANGULUS_OS_UNIX() 0
#endif

#if defined(__FreeBSD__)
	#define LANGULUS_OS_FREEBSD() 1
#else 
	#define LANGULUS_OS_FREEBSD() 0
#endif


namespace Langulus::Entity
{

	/// Close a shared library handle, unloading it										
	///	@param library - the library handle												
	#if LANGULUS_OS(WINDOWS)
		void UnloadSharedLibrary(HMODULE library) {
			FreeLibrary(library);
		}
	#elif LANGULUS_OS(LINUX)
		void UnloadSharedLibrary(void* library) {
			dlclose(library);
		}
	#else
		#error Unsupported OS
	#endif
	
	/// Load a shared library module															
	///	@param filename - the shared library to load									
	///	@return the shared library handle												
	SharedLibrary LoadSharedLibrary(const Path& filename) {
		SharedLibrary handle;
		Path file;

		// File prefix																		
		#if LANGULUS_OS(LINUX)
			file = "./lib";
		#endif

		file += "M";
		file += filename;

		#if LANGULUS(DEBUG)
			file += "d";
		#endif

		// File postfix																	
		#if LANGULUS_OS(WINDOWS)
			file += ".dll";
		#elif LANGULUS_OS(LINUX)
			file += ".so";
		#else
			#error Unsupported OS
		#endif

		// Make sure string ends with terminator									
		file = file.Terminate();

		// Load the library																
		#if LANGULUS_OS(WINDOWS)
			auto dll = LoadLibraryA(file.GetRaw());
		#elif LANGULUS_OS(LINUX)
			auto dll = dlopen(file.GetRaw(), RTLD_NOW);
		#else 
			#error Unsupported OS
		#endif

		if (!dll) {
			Logger::Error() << "Failed to load module " << file << " - file is missing or corrupted";
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("IModule::LOAD");
			#endif
			return {};
		}

		// Great success!																	
		Logger::Info() << "Module " << file << " loaded";

		// Get entry point from the dll												
		// Entry point is used to produce modules									
		#if LANGULUS_OS(WINDOWS)
			handle.mEntry = reinterpret_cast<Module::EntryPoint>(
				GetProcAddress(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER)));
		#elif LANGULUS_OS(LINUX)
			handle.mEntry = reinterpret_cast<ModuleEntryPointPtr>(
				dlsym(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER)));
		#else 
			#error Unsupported OS
		#endif	

		if (!handle.mEntry) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid linking point - unloading...";
			UnloadSharedLibrary(dll);
			return {};
		}

		// Get creator point from the dll											
		// Creator point is used to produce modules								
		#if LANGULUS_OS(WINDOWS)
			handle.mCreator = reinterpret_cast<Module::CreatePoint>(
				GetProcAddress(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR)));
		#elif LANGULUS_OS(LINUX)
			handle.mCreator = reinterpret_cast<ModuleCreatePointPtr>(
				dlsym(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR)));
		#else 
			#error Unsupported OS
		#endif

		if (!handle.mCreator) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid creator point - unloading...";
			UnloadSharedLibrary(dll);
			return {};
		}

		// Get info point from the dll												
		// Info point contains various information about module				
		// For example: module executional priority								
		#if LANGULUS_OS(WINDOWS)
			handle.mInfo = reinterpret_cast<Module::InfoPoint>(
				GetProcAddress(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_INFO)));
		#elif LANGULUS_OS(LINUX)
			handle.mInfo = reinterpret_cast<ModuleInfoPointPtr>(
				dlsym(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_INFO)));
		#else 
			#error Unsupported OS
		#endif

		if (!handle.mInfo) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_INFO) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid info point - unloading...";
			UnloadSharedLibrary(dll);
			return {};
		}

		// Get exit point from the dll												
		// Exit point unlinks and removed meta data								
		#if LANGULUS_OS(WINDOWS)
			handle.mExit = reinterpret_cast<Module::ExitPoint>(
				GetProcAddress(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER)));
		#elif LANGULUS_OS(LINUX)
			handle.mExit = reinterpret_cast<ModuleExitPointPtr>(
				dlsym(dll, LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER)));
		#else 
			#error Unsupported OS
		#endif

		if (!handle.mExit) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid exit point - unloading...";
			UnloadSharedLibrary(dll);
			return {};
		}

		return handle;
	}

	/// Default runtime component construction											
	Runtime::Runtime() {
		Logger::Verbose() << "Initializing...";
		Logger::Verbose() << "Initialized";
	}

	/// Runtime component destruction														
	Runtime::~Runtime() {
		while (!mLibraries.IsEmpty())
			UnloadSharedLibrary(mLibraries.GetValue(IndexFirst));
	}

	/// Create a module instance or return an already instantiated one			
	///	@param construct - module initialization construct							
	///	@return the new module instance													
	Module* Runtime::InstantiateModule(const Construct& construct) {
		const auto type = construct.GetType();
		auto module = GetModule(type);
		if (!module) {
			// A module instance doesn't exist yet, so instantiate			
			Logger::Warning()
				<< "Required module " << type
				<< " is not instanced - attempting to create module...";

			// Delegate any configuration to the module instance via the	
			// construct (the descriptor)												
			module = Module::CREATE(this, construct);
			mModules.Merge(info.mPriority, module.Get());
			mModules.Sort(IndexSmallest);
			Logger::Verbose()
				<< "Module " << info.mName
				<< " registered with priority " << info.mPriority;
			return module;

			if (!module) {
				Logger::Error()
					<< "Can't instantiate module " << type
					<< " - most likely due to bad initialization, or a missing DLL/SO file")
				Throw<Except::Construct>("Can't instantiate module ");
			}
		}

		return module;
	}

	/// Load a shared library for a module													
	///	@param filename - the file for the dynamic library							
	///	@return the module handle (OS dependent)										
	SharedLibrary Runtime::LoadSharedLibrary(const Path& filename) {
		// Clone path into our memory, just in case								
		auto path = filename.Clone();

		// Check if this library is already loaded								
		const auto preloaded = mLibraries.FindKeyIndex(path);
		if (preloaded) {
			// Never even attempt to load libraries more than once			
			return mLibraries.GetValue(preloaded);
		}

		// Load the library and retrieve entry, info, and exit points		
		const auto library = ::Langulus::Entity::LoadSharedLibrary(path);
		if (0 == library)
			return 0;

		try {
			// Link the module - this shall merge RTTI definitions			
			// It might throw if out of memory or on meta collision, while	
			// registering new types on the other side							
			linker();
		}
		catch (const Exception& exception) {
			// Probably meta data conflict											
			Logger::Error() << "Could not load: " << filename;
			Logger::Error() << "Exception: " << exception;
			exitp();
			Module::UNLOAD(library);
			throw;
		}

		// Get info and push to pending modules									
		mLibraries.Insert(path, library);
		return library;
	}

	/// Unload a DLL/SO extension module													
	///	@param module - the module handle to unload									
	void Runtime::UnloadSharedLibrary(SharedLibrary moduleHandle) {
		// Remove every instantiation made from this library					
		for (auto& group : mModules) {
			for (auto& module : group) {
				if (module->GetHandle() != moduleHandle)
					continue;
				group.Remove(module);
				//TODO check if deleted!
				break;
			}
		}

		// Remove every module occuring in the module registry				
		Module::UNLOAD(moduleHandle);
		mLibraries.RemoveValue(moduleHandle);
	}

	/// Get a DLL/SO extension module by data type										
	///	@param type - the module meta to get											
	///	@return the module instance														
	Module* Runtime::GetModule(DMeta type) {
		for (auto& module_group : mModules) {
			for (auto& mod : module_group) {
				if (mod->CastsTo(type))
					return mod;
			}
		}

		return nullptr;
	}

	/// Get a DLL/SO extension module by data type (const)							
	///	@param type - the module meta to get											
	///	@return the module instance														
	const Module* Runtime::GetModule(DMeta type) const {
		return const_cast<Runtime*>(this)->GetModule(type);
	}

	/// Get a DLL/SO extension module by token name										
	///	@param token - module name token													
	///	@return the module instance, or nullptr if not found						
	Module* Runtime::GetModule(const Token& token) {
		for (auto& pair : mModules) {
			for (auto& mod : pair) {
				if (token == mod->ClassMeta()->GetToken())
					return mod;
			}
		}

		return nullptr;
	}

	/// Get a DLL/SO extension module by token name										
	///	@param mname - module name token													
	///	@return the module instance, or nullptr if not found						
	const Module* Runtime::GetModule(const Text& token) const {
		return const_cast<Runtime*>(this)->GetModule(token);
	}

	/// Get number of active windows															
	///	@return the number of active windows											
	Count Runtime::GetNumberOfActiveWindows() const {
		const auto platform = GetModule(DataID::Reflect<AModulePlatform>());
		if (platform && platform->IsClassValid())
			return platform.As<AModulePlatform>()->GetNumberOfActiveWindows();
		return 0;
	}

	/// Get a file interface (loads MVFS if not loaded yet)							
	///	@param filename - the file to get												
	///	@return the file interface															
	AFile* Runtime::GetFile(const Path& filename) {
		auto fs = GetModule(DataID::Reflect<AModuleVFS>());
		if (!fs)
			fs = CreateModule(DataID::Reflect<AModuleVFS>());
		return fs.As<AModuleVFS>()->GetFile(filename);
	}

	/// Get a folder interface (loads MVFS if not loaded yet)						
	///	@param dirname - the directory to get											
	///	@return the folder interface														
	AFolder* Runtime::GetFolder(const Path& dirname) {
		auto fs = GetModule(DataID::Reflect<AModuleVFS>());
		if (!fs)
			fs = CreateModule(DataID::Reflect<AModuleVFS>());
		return fs.As<AModuleVFS>()->GetFolder(dirname);
	}

	/// Update the runtime, by updating each active module							
	///	@param dt - delta time between update calls									
	void Runtime::Update(Time dt) {
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
			Logger::Prompt() << owner << ": Waiting for command:\n";
			std::wstring line;
			std::getline(std::wcin, line);
			try {
				owner->RunCode(line.c_str());
			}
			catch (...) {

			}
		}
	}

} // namespace Langulus::Entity
