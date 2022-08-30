#include "AModuleVFS.hpp"

namespace PCFW
{

	/// Get the absolute data folder															
	///	@return the data folder																
	const Path& AModuleVFS::GetDataFolder() const noexcept {
		return mMainDataDirectory;
	}

	/// Get the absolute temporary folder													
	///	@return the temporary folder														
	const Path& AModuleVFS::GetTempFolder() const noexcept {
		return mMainCacheDirectory;
	}

	/// Get the absolute working directory													
	///	@return the working directory														
	const Path& AModuleVFS::GetMainFolder() const noexcept {
		return mWorkingDirectory;
	}

	/// Folder construction with a class overrider										
	///	@param classid - class id override												
	AFolder::AFolder(DMeta classid)
		: AUnitVFS {classid} {}

	const Path& AFolder::GetDirname() const noexcept {
		return mDirname;
	}

	FState AFolder::GetFolderState() const noexcept {
		return mState;
	}

	/// Get the hash of the folder path														
	///	@return the hash																		
	Hash AFolder::GetHash() const {
		if (mHashed)
			return mHash;
		return Hashed::SetHash(mDirname.GetHash());
	}

	/// Compare folder paths for equality													
	///	@param other - file to compare against											
	///	@return true if files have the same paths										
	bool AFolder::operator == (const AFolder& other) const noexcept {
		return CompareHash(other) && mDirname == other.mDirname;
	}

	/// Compare folder paths for inequality												
	///	@param other - folder to compare against										
	///	@return true if folders have different paths									
	bool AFolder::operator != (const AFolder& other) const noexcept {
		return !(*this == other);
	}

	/// File construction with a class overrider											
	///	@param classid - class id override												
	AFile::AFile(DMeta classid)
		: AUnitVFS {classid} {}

	/// Get the hash of the filename															
	///	@return the hash																		
	Hash AFile::GetHash() const {
		if (mHashed)
			return mHash;
		return Hashed::SetHash(mFilename.GetHash());
	}

	/// Compare filenames for equality														
	///	@param other - file to compare against											
	///	@return true if files have the same filename									
	bool AFile::operator == (const AFile& other) const noexcept {
		return CompareHash(other) && mFilename == other.mFilename;
	}

	/// Compare filenames for inequality													
	///	@param other - file to compare against											
	///	@return true if files have different filenames								
	bool AFile::operator != (const AFile& other) const noexcept {
		return !(*this == other);
	}

	/// Serialize to binary and write a block to the file								
	///	@param block - the block to serialize to binary								
	///	@return the number of written bytes												
	pcptr AFile::WriteBlock(const Block& block) {
		const auto serialized = pcSerialize<Bytes>(block);
		return WriteBytes(serialized);
	}

	/// Deserialize from binary and output a block										
	///	@param block - [out] the block to deserialize into							
	///	@return the number of read bytes													
	pcptr AFile::ReadBlock(Block& block) {
		Bytes temporary;
		return Flow::Detail::DeserializeBlockFromBinary<true>(
			temporary, block, 0, mHeader, 
			[&](Bytes& buffer, pcptr count) {
				auto segment = buffer.Extend(count);
				auto segmentBlock = Block::From(segment.GetRaw(), segment.GetCount());
				ReadBytes(segmentBlock);
			}
		);
	}

	/// Get the relative (to the data directory) path (without filename)			
	///	@return the relative path (null-terminated)									
	const Path& AFile::GetRelativePath() const noexcept {
		return mRelativePath;
	}

	/// Get the filename (without any path)												
	///	@return the filename																	
	const Path& AFile::GetFilename() const noexcept {
		return mFilename;
	}

	/// Get the filename (without any path and extension)								
	///	@return the filename																	
	Path AFile::GetFilenameWithoutExtension() const noexcept {
		auto fn = GetFilename();
		pcptr firstDot = 0;
		if (fn.FindOffset(".", firstDot))
			return fn.Crop(0, firstDot);
		return fn;
	}

	/// Get GetRelativePath() / GetFilename()												
	///	@return the relative filename														
	const Path& AFile::GetRelativeFilename() const noexcept {
		return mRelativeFilename;
	}

	/// Get the part after the last '.' in the filename (always lowercase)		
	///	@return the file extension															
	const Text& AFile::GetFileExtension() const noexcept {
		return mFileExtension;
	}

	/// Get the file type, if extension has been recognized by the reflection	
	///	@return the resolved type behind the file extension						
	DMeta AFile::GetFileType() const noexcept {
		return mFormat;
	}

	/// The file state with which last file opening occured with					
	///	@return the last file state														
	FState AFile::GetFileState() const noexcept {
		return mState;
	}

	/// Get the file size if existent (in bytes)											
	///	@return file size in bytes															
	pcptr AFile::GetFileSize() const noexcept {
		return mByteCount;
	}

	/// Check if file exists																	
	///	@return true if file exists														
	bool AFile::Exists() const noexcept {
		return mExists;
	}

} // namespace PCFW
