#include "Module.hpp"

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

	/// Load a library in RAM																	
	///	@param name - the filename to load												
	///	@return a symbolic handle for the module										
	SharedLibrary pcLoadLibrary(const Path& name) {
		#if LANGULUS_OS(WINDOWS)
			return pcP2N(LoadLibraryA(name.GetRaw()));
		#elif LANGULUS_OS(LINUX)
			return pcP2N(dlopen(name.GetRaw(), RTLD_NOW));
		#else 
			#error Unsupported OS
		#endif	
	}

	/// Get the library link point, which adds meta data								
	///	@param library - the library handle												
	///	@return the link callback															
	ModuleLinkPointPtr pcLibGetLinker(SharedLibrary library) {
		#if LANGULUS_OS(WINDOWS)
			return reinterpret_cast<ModuleLinkPointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER)));
		#elif LANGULUS_OS(LINUX)
			return reinterpret_cast<ModuleLinkPointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER)));
		#else 
			#error Unsupported OS
		#endif	
	}

	/// Get the library entry point, producing an AModule								
	///	@param library - the library handle												
	///	@return the entry callback															
	ModuleCreatePointPtr pcLibGetCreator(SharedLibrary library) {
		#if LANGULUS_OS(WINDOWS)
			return reinterpret_cast<ModuleCreatePointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR)));
		#elif LANGULUS_OS(LINUX)
			return reinterpret_cast<ModuleCreatePointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR)));
		#else 
			#error Unsupported OS
		#endif	
	}

	/// Get the library info point															
	///	@param library - the library handle												
	///	@return the info callback															
	ModuleInfoPointPtr pcLibGetInfo(SharedLibrary library) {
		#if LANGULUS_OS(WINDOWS)
			return reinterpret_cast<ModuleInfoPointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_INFO)));
		#elif LANGULUS_OS(LINUX)
			return reinterpret_cast<ModuleInfoPointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_INFO)));
		#else 
			#error Unsupported OS
		#endif	
	}

	/// Get the library exit point, which removes meta definitions					
	///	@param library - the library handle												
	///	@return the exit callback															
	ModuleExitPointPtr pcLibGetExiter(SharedLibrary library) {
		#if LANGULUS_OS(WINDOWS)
			return reinterpret_cast<ModuleExitPointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER)));
		#elif LANGULUS_OS(LINUX)
			return reinterpret_cast<ModuleExitPointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER)));
		#else 
			#error Unsupported OS
		#endif	
	}

	/// Close a shader library, unloading it from RAM									
	///	@param library - the library handle												
	void pcLibClose(SharedLibrary library) {
		#if LANGULUS_OS(WINDOWS)
			FreeLibrary(reinterpret_cast<HMODULE>(library));
		#elif LANGULUS_OS(LINUX)
			dlclose(const_cast<void*>(pcN2P(library)));
		#else 
			#error Unsupported OS
		#endif	
	}


	/// Construction																				
	///	@param classid - the class type for the module								
	///	@param system - the owner of the module										
	///	@param dllhandle - the handle to the shared library						
	Module::Module(DMeta classid, CRuntime* system, SharedLibrary dllhandle) noexcept
		: Resolvable {classid}
		, mLibraryHandle {dllhandle}
		, mRuntime {system} {}

	/// Get the library handle																	
	///	@return the handle																	
	SharedLibrary Module::GetHandle() const noexcept {
		return mLibraryHandle;
	}

	/// Load a shared library module															
	///	@param filename - the module to load											
	///	@param link - the module entry callback										
	///	@param info - the module info callback											
	///	@param unload - the module exit callback										
	///	@return a symbolic handle for the module										
	SharedLibrary Module::LOAD(const Path& filename
		, ModuleLinkPointPtr& link
		, ModuleInfoPointPtr& info
		, ModuleExitPointPtr& unload) {
		Path file;

		// File prefix																		
		#if LANGULUS_OS(LINUX)
			file = "./lib";
		#endif

		file += "M";
		file += filename;

		#if LANGULUS_DEBUG()
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
		SharedLibrary dll = pcLoadLibrary(file.GetRaw());
		if (dll == 0) {
			Logger::Error() << "Failed to load module " << file << " - file is missing or corrupted";
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("IModule::LOAD");
			#endif
			return 0;
		}

		// Great success!																	
		Logger::Info() << "Module " << file << " loaded";

		// Get entry point from the dll												
		// Entry point is used to produce modules									
		link = pcLibGetLinker(dll);
		if (!link) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid linking point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		// Get creator point from the dll											
		// Creator point is used to produce modules								
		auto create = pcLibGetCreator(dll);
		if (!create) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid creator point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		// Get info point from the dll												
		// Info point contains various information about module				
		// For example: module executional priority								
		info = pcLibGetInfo(dll);
		if (!info) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_INFO) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid info point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		// Get exit point from the dll												
		// Exit point unlinks and removed meta data								
		unload = pcLibGetExiter(dll);
		if (!unload) {
			#if LANGULUS_OS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER) ")");
			#endif
			Logger::Error() << "Module " << file << " has no valid exit point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		return dll;
	}

	/// Produce a module from a loaded library											
	///	@param system - the system that will own the module instance			
	///	@param library - the library handle to use as producer					
	///	@return a pointer to the newly created module								
	Ptr<Module> Module::CREATE(const Ptr<Runtime>& system, SharedLibrary library) {
		auto entry = pcLibGetCreator(library);
		return entry(system.Get(), library);
	}

	/// Unload a DLL/SO extension module													
	///	@param library - the library handle to use as producer					
	void Module::UNLOAD(SharedLibrary library) {
		pcLibClose(library);
	}

	/// Get info from a library handle														
	///	@param library - the library handle to use as producer					
	///	@return a reference to the module info structure							
	const ModuleInfo& Module::INFO(SharedLibrary library) {
		auto infoCall = pcLibGetInfo(library);
		return infoCall();
	}

} // namespace Langulus::Entity
