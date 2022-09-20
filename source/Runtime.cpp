///																									
/// Langulus::Entity																				
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>							
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
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
	#include <dlfcn.h>
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


	/// Runtime construction																	
	///	@param owner - the owner of the runtime										
	Runtime::Runtime(Entity* owner) noexcept
		: mOwner {owner} {
		Logger::Verbose() << "Initializing...";
		Logger::Verbose() << "Initialized";
	}

	/// Runtime destruction																		
	Runtime::~Runtime() {
		while (!mLibraries.IsEmpty())
			UnloadSharedLibrary(mLibraries.GetValue(IndexFirst));
	}

	/// Create a module instance or return an already instantiated one			
	///	@param construct - module initialization construct							
	///	@return the new module instance													
	Module* Runtime::InstantiateModule(const Construct& construct) {
		const auto type = construct.GetType();
		if (!type->mProducer->Is<Runtime>()) {
			Logger::Error()
				<< "Runtime can't instantiate " << type << " - it's not a module";
			return nullptr;
		}

		// Check if module is already instantiated								
		auto& foundModules = GetModules(type);
		if (!foundModules.IsEmpty())
			return foundModules[0];

		// What library produces the module/data?									
		auto dependency = mDependencies.FindKeyIndex(type);
		if (!dependency) {
			Logger::Error()
				<< "Module " << type << "'s dependency is not known";
			return nullptr;
		}

		// A module instance doesn't exist yet, so instantiate it			
		Logger::Verbose()
			<< "Module " << type << " is not instantiated yet"
			<< " - attempting to create it...";
		auto& library = mDependencies.GetValue(dependency);

		// Use the creation point of the library to instantiate module		
		const auto& info = library.mInfo();
		auto module = library.mCreator(this, construct);
		if (!module) {
			Logger::Error()
				<< "Module " << type << "'s creator didn't provide a module";
			return nullptr;
		}

		// Register the module in the various maps, for fast access			
		try {
			mModules[info.mPriority] << module;
			mInstantiations[library] << module;
			mModulesByType[module->GetType()] << module;
		}
		catch (...) {
			Logger::Error()
				<< "Registering module " << type << " failed";

			// Make sure we end up in an invariant state							
			mModules[info.mPriority].RemoveValue(module);
			if (mModules[info.mPriority].IsEmpty())
				mModules.RemoveKey(info.mPriority);

			mInstantiations[library].RemoveValue(module);
			if (mInstantiations[library].IsEmpty())
				mInstantiations.RemoveKey(library);

			mModulesByType[module->GetType()].RemoveValue(module);
			if (mModulesByType[module->GetType()].IsEmpty())
				mModulesByType.RemoveKey(module->GetType());

			delete module;
			return nullptr;
		}

		// Done, if reached																
		Logger::Verbose()
			<< "Module " << info.mName
			<< " registered with priority " << info.mPriority;
		return module;
	}

	/// Load a shared library for a module													
	///	@param filename - the file for the dynamic library							
	///	@return the module handle (OS dependent)										
	typename Runtime::SharedLibrary Runtime::LoadSharedLibrary(const Path& filename) {
		// Check if this library is already loaded								
		const auto preloaded = mLibraries.FindKeyIndex(filename);
		if (preloaded) {
			// Never even attempt to load libraries more than once			
			return mLibraries.GetValue(preloaded);
		}

		// File prefix																		
		Path path;
		#if LANGULUS_OS(LINUX)
			path += "./lib";
		#endif

		path += "M";
		path += filename;

		#if LANGULUS(DEBUG)
			path += "d";
		#endif

		// File postfix																	
		#if LANGULUS_OS(WINDOWS)
			path += ".dll";
		#elif LANGULUS_OS(LINUX)
			path += ".so";
		#else
			#error Unsupported OS
		#endif

		// Make sure string ends with terminator									
		path = path.Terminate();

		// Load the library																
		#if LANGULUS_OS(WINDOWS)
			const auto dll = LoadLibraryA(path.GetRaw());
		#elif LANGULUS_OS(LINUX)
			const auto dll = dlopen(path.GetRaw(), RTLD_NOW);
		#else 
			#error Unsupported OS
		#endif

		if (!dll) {
			Logger::Error() << "Failed to load module `" << path << "` - file is missing or corrupted; Error code: ";
			#if LANGULUS_OS(WINDOWS)
				Logger::Append() << ::GetLastError();
			#endif
			return {};
		}

		// Great success!																	
		Logger::Info() << "Module " << path << " loaded";
		SharedLibrary library;
		static_assert(sizeof(library.mHandle) == sizeof(dll), "Size mismatch");
		library.mHandle = reinterpret_cast<decltype(library.mHandle)>(dll);

		// Get entry, creator, info and exit points from the library		
		#if LANGULUS_OS(WINDOWS)
			library.mEntry = reinterpret_cast<Module::EntryPoint>(
				GetProcAddress(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
			library.mCreator = reinterpret_cast<Module::CreatePoint>(
				GetProcAddress(dll, LANGULUS_MODULE_CREATE_TOKEN()));
			library.mInfo = reinterpret_cast<Module::InfoPoint>(
				GetProcAddress(dll, LANGULUS_MODULE_INFO_TOKEN()));
			library.mExit = reinterpret_cast<Module::ExitPoint>(
				GetProcAddress(dll, LANGULUS_MODULE_EXIT_TOKEN()));
		#elif LANGULUS_OS(LINUX)
			library.mEntry = reinterpret_cast<Module::EntryPoint>(
				dlsym(dll, LANGULUS_MODULE_ENTRY_TOKEN()));
			library.mCreator = reinterpret_cast<Module::CreatePoint>(
				dlsym(dll, LANGULUS_MODULE_CREATE_TOKEN()));
			library.mInfo = reinterpret_cast<Module::InfoPoint>(
				dlsym(dll, LANGULUS_MODULE_INFO_TOKEN()));
			library.mExit = reinterpret_cast<Module::ExitPoint>(
				dlsym(dll, LANGULUS_MODULE_EXIT_TOKEN()));
		#else 
			#error Unsupported OS
		#endif	

		if (!library.mEntry) {
			Logger::Error()
				<< "Module `" << path << "` has no valid entry point - "
				<< "the function " LANGULUS_MODULE_ENTRY_TOKEN() " is missing";
			UnloadSharedLibrary(library);
			return {};
		}

		if (!library.mCreator) {
			Logger::Error()
				<< "Module `" << path << "` has no valid instantiation point - "
				<< "the function " LANGULUS_MODULE_CREATE_TOKEN() " is missing";
			UnloadSharedLibrary(library);
			return {};
		}

		if (!library.mInfo) {
			Logger::Error()
				<< "Module `" << path << "` has no valid information point - "
				<< "the function " LANGULUS_MODULE_INFO_TOKEN() " is missing";
			UnloadSharedLibrary(library);
			return {};
		}

		if (!library.mExit) {
			Logger::Error()
				<< "Module `" << path << "` has no valid exit point - "
				<< "the function " LANGULUS_MODULE_EXIT_TOKEN() " is missing";
			UnloadSharedLibrary(library);
			return {};
		}

		// Link the module - this shall merge RTTI definitions				
		// It might throw if out of memory or on meta collision, while		
		// registering new types on the other side								
		MetaList registeredTypes;
		try {
			registeredTypes = library.mEntry();

			mLibraries.Insert(path, library);
			for (auto externalType : registeredTypes)
				mDependencies.Insert(externalType, library);
		}
		catch (...) {
			// Make sure we end up in an invariant state							
			Logger::Error() << "Could not enter `" << path << "` due to an exception";
			mLibraries.RemoveKey(path);
			for (auto externalType : registeredTypes)
				mDependencies.RemoveKey(externalType);
			UnloadSharedLibrary(library);
			return {};
		}

		return library;
	}

	/// Unload a DLL/SO extension module													
	///	@param module - the module handle to unload									
	void Runtime::UnloadSharedLibrary(const SharedLibrary& module) {
		if (module.mHandle == 0)
			return;

		const MetaList unregisteredTypes = module.mExit();
		for (auto externalType : unregisteredTypes)
			mDependencies.RemoveKey(externalType);

		#if LANGULUS_OS(WINDOWS)
			::Langulus::Entity::UnloadSharedLibrary(
				reinterpret_cast<HMODULE>(module.mHandle));
		#elif LANGULUS_OS(LINUX)
			::Langulus::Entity::UnloadSharedLibrary(
				reinterpret_cast<void*>(module.mHandle));
		#else 
			#error Unsupported OS
		#endif
	}

	/// Get a module instance by type														
	///	@param type - the type to search for											
	///	@return the module instance														
	const ModuleList& Runtime::GetModules(DMeta type) const noexcept {
		auto found = mDependencies.FindKeyIndex(type);
		if (found)
			return mInstantiations[mDependencies.GetValue(found)];

		static const ModuleList emptyFallback {};
		return emptyFallback;
	}

	/// Get a module instance by type token												
	///	@param token - type token															
	///	@return the module instance, or nullptr if not found						
	const ModuleList& Runtime::GetModules(const Token& token) const noexcept {
		return GetModules(RTTI::Database.GetMetaData(token));
	}

	/// Update the runtime, by updating all module instantiations					
	///	@param dt - delta time between update calls									
	void Runtime::Update(Time dt) {
		for (auto pair : mInstantiations) {
			for (auto module : pair.mValue)
				module->Update(dt);
		}
	}

} // namespace Langulus::Entity
