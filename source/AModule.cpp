#include "AModule.hpp"
#include <PCFW.OS.hpp>

namespace PCFW
{

	/// Construction																				
	///	@param classid - the class type for the module								
	///	@param system - the owner of the module										
	///	@param dllhandle - the handle to the shared library						
	AModule::AModule(DMeta classid, CRuntime* system, PCLIB dllhandle) noexcept
		: AContext {classid}
		, mLibraryHandle {dllhandle}
		, mRuntime {system} {
		// Each module is constructed as invalid									
		ClassInvalidate();
	}

	/// Get the library handle																	
	///	@return the handle																	
	PCLIB AModule::GetHandle() const noexcept {
		return mLibraryHandle;
	}

	/// Load a library in RAM																	
	///	@param name - the filename to load												
	///	@return a symbolic handle for the module										
	PCLIB pcLoadLibrary(const Path& name) {
		#if LANGULUS_OS_IS(WINDOWS)
			return pcP2N(LoadLibraryA(name.GetRaw()));
		#elif LANGULUS_OS_IS(LINUX)
			return pcP2N(dlopen(name.GetRaw(), RTLD_NOW));
		#else 
			#error Unsupported OS - implement yourself, or give us money
		#endif	
	}

	/// Get the library link point, which adds meta data								
	///	@param library - the library handle												
	///	@return the link callback															
	ModuleLinkPointPtr pcLibGetLinker(PCLIB library) {
		#if LANGULUS_OS_IS(WINDOWS)
			return reinterpret_cast<ModuleLinkPointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER)));
		#elif LANGULUS_OS_IS(LINUX)
			return reinterpret_cast<ModuleLinkPointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER)));
		#else 
			#error Unsupported OS - implement yourself, or give us money
		#endif	
	}

	/// Get the library entry point, producing an AModule								
	///	@param library - the library handle												
	///	@return the entry callback															
	ModuleCreatePointPtr pcLibGetCreator(PCLIB library) {
		#if LANGULUS_OS_IS(WINDOWS)
			return reinterpret_cast<ModuleCreatePointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR)));
		#elif LANGULUS_OS_IS(LINUX)
			return reinterpret_cast<ModuleCreatePointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR)));
		#else 
			#error Unsupported OS - implement yourself, or give us money
		#endif	
	}

	/// Get the library info point															
	///	@param library - the library handle												
	///	@return the info callback															
	ModuleInfoPointPtr pcLibGetInfo(PCLIB library) {
		#if LANGULUS_OS_IS(WINDOWS)
			return reinterpret_cast<ModuleInfoPointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_INFO)));
		#elif LANGULUS_OS_IS(LINUX)
			return reinterpret_cast<ModuleInfoPointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_INFO)));
		#else 
			#error Unsupported OS - implement yourself, or give us money
		#endif	
	}

	/// Get the library exit point, which removes meta definitions					
	///	@param library - the library handle												
	///	@return the exit callback															
	ModuleExitPointPtr pcLibGetExiter(PCLIB library) {
		#if LANGULUS_OS_IS(WINDOWS)
			return reinterpret_cast<ModuleExitPointPtr>(
				GetProcAddress(reinterpret_cast<HMODULE>(library), LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER)));
		#elif LANGULUS_OS_IS(LINUX)
			return reinterpret_cast<ModuleExitPointPtr>(
				dlsym(const_cast<void*>(pcN2P(library)), LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER)));
		#else 
			#error Unsupported OS - implement yourself, or give us money
		#endif	
	}

	/// Close a shader library, unloading it from RAM									
	///	@param library - the library handle												
	void pcLibClose(PCLIB library) {
		#if LANGULUS_OS_IS(WINDOWS)
			FreeLibrary(reinterpret_cast<HMODULE>(library));
		#elif LANGULUS_OS_IS(LINUX)
			dlclose(const_cast<void*>(pcN2P(library)));
		#else 
			#error Unsupported OS - implement yourself, or give us money
		#endif	
	}

	/// Load a shared library module															
	///	@param filename - the module to load											
	///	@param link - the module entry callback										
	///	@param info - the module info callback											
	///	@param unload - the module exit callback										
	///	@return a symbolic handle for the module										
	PCLIB AModule::LOAD(const Path& filename
		, ModuleLinkPointPtr& link
		, ModuleInfoPointPtr& info
		, ModuleExitPointPtr& unload) {
		Path file;

		// File prefix																		
		#if LANGULUS_OS_IS(LINUX)
			file = "./lib";
		#endif

		file += "M";
		file += filename;

		#if LANGULUS_DEBUG()
			file += "d";
		#endif

		// File postfix																	
		#if LANGULUS_OS_IS(WINDOWS)
			file += ".dll";
		#elif LANGULUS_OS_IS(LINUX)
			file += ".so";
		#else
			#error Unsupported OS - implement yourself, or give us money
		#endif

		// Make sure string ends with terminator									
		file = file.Terminate();

		// Load the library																
		PCLIB dll = pcLoadLibrary(file.GetRaw());
		if (dll == 0) {
			pcLogFuncError << "Failed to load module " << file << " - file is missing or corrupted";
			#if LANGULUS_OS_IS(WINDOWS)
				OSGetLastError("IModule::LOAD");
			#endif
			return 0;
		}

		// Great success!																	
		pcLogFuncInfo << "Module " << file << " loaded";

		// Get entry point from the dll												
		// Entry point is used to produce modules									
		link = pcLibGetLinker(dll);
		if (!link) {
			#if LANGULUS_OS_IS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_LINKER) ")");
			#endif
			pcLogFuncError << "Module " << file << " has no valid linking point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		// Get creator point from the dll											
		// Creator point is used to produce modules								
		auto create = pcLibGetCreator(dll);
		if (!create) {
			#if LANGULUS_OS_IS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_CREATOR) ")");
			#endif
			pcLogFuncError << "Module " << file << " has no valid creator point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		// Get info point from the dll												
		// Info point contains various information about module				
		// For example: module executional priority								
		info = pcLibGetInfo(dll);
		if (!info) {
			#if LANGULUS_OS_IS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_INFO) ")");
			#endif
			pcLogFuncError << "Module " << file << " has no valid info point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		// Get exit point from the dll												
		// Exit point unlinks and removed meta data								
		unload = pcLibGetExiter(dll);
		if (!unload) {
			#if LANGULUS_OS_IS(WINDOWS)
				OSGetLastError("GetProcAddress(" LANGULUS_TOSTRING(LANGULUS_MODULE_DESTROYER) ")");
			#endif
			pcLogFuncError << "Module " << file << " has no valid exit point - unloading...";
			pcLibClose(dll);
			return 0;
		}

		return dll;
	}

	/// Produce a module from a loaded library											
	///	@param system - the system that will own the module instance			
	///	@param library - the library handle to use as producer					
	///	@return a pointer to the newly created module								
	Ptr<AModule> AModule::CREATE(const Ptr<CRuntime>& system, PCLIB library) {
		auto entry = pcLibGetCreator(library);
		return entry(system.Get(), library);
	}

	/// Unload a DLL/SO extension module													
	///	@param library - the library handle to use as producer					
	void AModule::UNLOAD(PCLIB library) {
		pcLibClose(library);
	}

	/// Get info from a library handle														
	///	@param library - the library handle to use as producer					
	///	@return a reference to the module info structure							
	const ModuleInfo& AModule::INFO(PCLIB library) {
		auto infoCall = pcLibGetInfo(library);
		return infoCall();
	}

} // namespace PCFW
