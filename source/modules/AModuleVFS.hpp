#pragma once
#include "../AModule.hpp"
#include "../AUnit.hpp"

namespace PCFW
{

	class AFile;
	class AFolder;

	///																								
	///	ABSTRACT FILE SYSTEM INTERFACE													
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleVFS : public AModule {
		REFLECT(AModuleVFS);
	public:
		using AModule::AModule;

		NOD() virtual AFile* GetFile(const Path&) = 0;
		NOD() virtual AFolder* GetFolder(const Path&) = 0;

		NOD() const Path& GetDataFolder() const noexcept;
		NOD() const Path& GetTempFolder() const noexcept;
		NOD() const Path& GetMainFolder() const noexcept;

	protected:
		Path mWorkingDirectory;
		Path mMainDataDirectory;
		Path mMainCacheDirectory;
	};


	///																								
	///	FILE STATE																				
	///																								
	class LANGULUS_MODULE(FRAMEWORK) FileState : public TNamed<FileState, pcu8> {
		REFLECT(FileState);
	public:
		enum Enum : pcu8 {
			// By default, a file is opened for both binary reading			
			// & writing without append and without dynamic watch				
			Auto = 0,
			Read = 1,
			Write = 2,
			Append = 4,
			Create = 8,
			Binary = 16,

			// A dynamic file is watched for external changes and				
			// automatically reloaded upon such change							
			Dynamic = 32,

			BinaryWrite = Binary | Write,
			BinaryRead = Binary | Read
		};

		/// Get named token																		
		///	@param index - the index to get												
		///	@return the token																	
		static constexpr LiteralText GetToken(pcptr index) {
			constexpr LiteralText names[] = {
				"fsAuto",
				"fsRead",
				"fsWrite",
				"fsAppend",
				"fsCreate",
				"fsBinary",
				"fsDynamic"
			};

			if (index >= sizeof(names) / sizeof(LiteralText))
				throw Except::BadAccess("Named array out of bounds");
			return names[index];
		}

		using TNamed::TNamed;

		constexpr FileState(FileState::Enum initial)
			: TNamed(initial) {}
	};

	using FState = FileState;


	///																								
	///	ABSTRACT FILE SYSTEM COMPONENT INTERFACE										
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitVFS : public AUnit {
		REFLECT(AUnitVFS);
		using AUnit::AUnit;
	};


	///																								
	///	ABSTRACT FILE INTERFACE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AFile : public AUnitVFS, public Hashed {
		REFLECT(AFile);

		struct ScopedClose {
			ScopedClose() = delete;
			ScopedClose(const ScopedClose&) = delete;
			ScopedClose(ScopedClose&& f) noexcept
				: file{ f.file } { f.file = nullptr; }
			ScopedClose(AFile* f) noexcept
				: file{ f } {}
			~ScopedClose() {
				if (file)
					file->Close();
			}

			inline operator bool() const noexcept {
				return file != nullptr;
			}

			AFile* file;
		};

		AFile(DMeta);

		NOD() Hash GetHash() const override;
		bool operator == (const ME&) const noexcept;
		bool operator != (const ME&) const noexcept;

		NOD() virtual ScopedClose Open(const FState&) = 0;
		virtual void Close() = 0;
		virtual pcptr ReadBytes(Block&) = 0;
		virtual pcptr WriteBytes(const Block&) = 0;
		NOD() virtual Path GetAbsoluteFilename() const = 0;

		pcptr WriteBlock(const Block&);
		pcptr ReadBlock(Block&);

		template<class T>
		pcptr WriteInternalID(const T&);
		template<class T>
		pcptr ReadInternalID(T&);

		template<class T>
		pcptr Write(const T&);
		template<class T>
		pcptr Read(T&);

		template<class T>
		AFile& operator << (const T&);
		template<class T>
		AFile& operator >> (T&);

		template<class T>
		AFile& WriteHeader();
		template<class T>
		AFile& ReadHeader();

		NOD() const Path& GetRelativePath() const noexcept;
		NOD() const Path& GetFilename() const noexcept;
		NOD() Path GetFilenameWithoutExtension() const noexcept;
		NOD() const Path& GetRelativeFilename() const noexcept;
		NOD() const Text& GetFileExtension() const noexcept;
		NOD() DMeta GetFileType() const noexcept;
		NOD() FState GetFileState() const noexcept;
		NOD() pcptr GetFileSize() const noexcept;
		NOD() bool Exists() const noexcept;

		template<class OUTPUT>
		NOD() OUTPUT ReadAs() const;

	protected:
		// Path, relative to the data directory of the implementation		
		Path mRelativePath;
		// The <relativePath>/<filename>												
		Path mRelativeFilename;
		// The filename (after the last '/')										
		Path mFilename;
		// File extension as a lowercase string									
		Text mFileExtension;
		// Resolved format of the file, if resolvable							
		DMeta mFormat = nullptr;
		// Size of the file, in bytes													
		pcptr mByteCount = 0;
		// Number of bytes read															
		pcptr mProgress = 0;
		// Does file exist?																
		bool mExists = false;
		// File state																		
		FState mState = FState::Auto;
		// PCF file header																
		::PCFW::Flow::Detail::Header mHeader;
	};


	///																								
	///	ABSTRACT FOLDER INTERFACE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AFolder : public AUnitVFS, public Hashed {
		REFLECT(AFolder);

		AFolder(DMeta);

		NOD() virtual AFile* GetFile(const Path&) = 0;
		NOD() virtual AFolder* GetFolder(const Path&) = 0;
		NOD() virtual Path GetAbsoluteDirname() const = 0;

		NOD() Hash GetHash() const override;
		bool operator == (const ME&) const noexcept;
		bool operator != (const ME&) const noexcept;

		NOD() const Path& GetDirname() const noexcept;
		NOD() FState GetFolderState() const noexcept;

	protected:
		// Directory name																	
		Path mDirname;
		// Folder state																	
		FState mState = FState::Auto;
		// Does folder exist																
		bool mExists = false;
	};

} // namespace PCFW

/// Declare constants for GASM and RTTI													
LANGULUS_DECLARE_CONSTANT(AutoFS, FState(FState::Auto), "Default file state");
LANGULUS_DECLARE_CONSTANT(ReadFS, FState(FState::Read), "File reading state");
LANGULUS_DECLARE_CONSTANT(WriteFS, FState(FState::Write), "File write state");
LANGULUS_DECLARE_CONSTANT(AppendFS, FState(FState::Append), "File append state");
LANGULUS_DECLARE_CONSTANT(CreateFS, FState(FState::Create), "File create state");
LANGULUS_DECLARE_CONSTANT(BinaryFS, FState(FState::Binary), "File binary state");
LANGULUS_DECLARE_CONSTANT(DynamicFS, FState(FState::Dynamic), "File dynamic state");

LANGULUS_DECLARE_EXCEPTION(FileIO);

#include "AModuleVFS.inl"