namespace PCFW
{

	/// Write internal ID																		
	template<class T>
	pcptr AFile::WriteInternalID(const T& what) {
		return WriteBytes(Bytes{ what });
	}

	/// Read internal ID																			
	template<class T>
	pcptr AFile::ReadInternalID(T& output) {
		Bytes temporary;
		return Flow::Detail::DeserializeInternalFromBinary(
			temporary, output, 0, mHeader, [&](Bytes& buffer, pcptr count) {
				auto segment = buffer.Extend(count);
				auto segmentBlock = Block::From(segment.GetRaw(), segment.GetCount());
				ReadBytes(segmentBlock);
			}
		);
	}

	/// Write anything reflected to the file												
	///	@tparam T - the type of data to write											
	///	@param what - the data instance to write										
	///	@return the number of written bytes												
	template<class T>
	pcptr AFile::Write(const T& what) {
		if constexpr (pcHasBase<T, Block>)
			return WriteBlock(what);
		else if constexpr (pcHasBase<T, InternalID>)
			return WriteInternalID<T>(what);
		else if constexpr (pcIsPOD<T>)
			return WriteBytes(Bytes{ what });
		else LANGULUS_ASSERT("Can't write unsupported type to file");
	}

	/// Read anything reflected from the file												
	///	@tparam T - the type of data to read											
	///	@param what - [out] data instance to fill										
	///	@return the number of bytes that were read									
	template<class T>
	pcptr AFile::Read(T& what) {
		if constexpr (pcHasBase<T, Block>)
			return ReadBlock(what);
		else if constexpr (pcHasBase<T, InternalID>)
			return ReadInternalID<T>(what);
		else if constexpr (pcIsPOD<T>) {
			auto interface = Block::From(what);
			return ReadBytes(interface);
		}
		else LANGULUS_ASSERT("Can't read unsupported type from file");
	}

	/// Write anything reflected to the file												
	///	@attention this doesn't write any header 										
	///	@tparam T - the type of data to write											
	///	@param what - the data instance to write										
	///	@return a reference to the file for chaining									
	template<class T>
	AFile& AFile::operator << (const T& what) {
		Write(what);
		return *this;
	}

	/// Read anything reflected from the file												
	///	@attention this doesn't check any header 										
	///	@tparam T - the type of data to read											
	///	@param what - [out] data instance to fill										
	///	@return a reference to the file for chaining									
	template<class T>
	AFile& AFile::operator >> (T& what) {
		Read(what);
		return *this;
	}

	/// Write a binary file header															
	///	@tparam T - the type of the file													
	///	@return a reference to the file for chaining									
	template<class T>
	AFile& AFile::WriteHeader() {
		mHeader = {};
		mFormat = MetaData::Of<T>();
		Write(mHeader);
		Write(mFormat->GetID());
		return *this;
	}

	/// Read a binary file header, throw if type doesn't match T					
	///	@tparam T - the type of the file to allow										
	///	@return a reference to the file for chaining									
	template<class T>
	AFile& AFile::ReadHeader() {
		Read(mHeader);
		DataID fileFormat;
		Read(fileFormat);
		if (DataID::Of<T> != fileFormat)
			throw Except::FileIO(pcLogSelfError << "File type doesn't match header");
		mFormat = fileFormat.GetMeta();
		return *this;
	}

	/// Read the whole file as a typed container											
	///	@tparam OUTPUT - the type to interpret bytes as								
	///						  you can use anything that passes IsText, or			
	///						  a TAny of some type											
	///	@return the container with the parsed file									
	template<class OUTPUT>
	OUTPUT AFile::ReadAs() const {
		static_assert(Dense<OUTPUT>, "OUTPUT must be dense");
		auto thisMutable = const_cast<AFile*>(this);
		auto openedFile = thisMutable->Open(FState::BinaryRead);
		Bytes buffer;
		buffer.Allocate(mByteCount, false, true);
		thisMutable->ReadBytes(buffer);
		if constexpr (IsText<OUTPUT>)
			return OUTPUT {
				buffer.Get<const char*>(),
				mByteCount
			};
		else if constexpr (pcIsDeep<OUTPUT>) {
			static const OUTPUT prototype;
			if (!prototype.GetMeta())
				throw Except::FileIO(pcLogSelfError << "The output container is untyped");

			return OUTPUT {
				Block {
					DState::Default,
					prototype.GetMeta(),
					mByteCount / prototype.GetStride(),
					buffer.GetRaw()
				}
			};
		}
		else LANGULUS_ASSERT("Can't ReadAs file in this format");
	}

} // namespace PCFW
