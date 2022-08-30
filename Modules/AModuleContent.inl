namespace PCFW
{

	/// Get named token for blend mode														
	///	@param index - the index to get													
	///	@return the token																		
	constexpr LiteralText BlendMode::GetToken(pcptr index) {
		if (index >= pcptr(Enum::Counter))
			throw Except::BadAccess("Blending mode out of bounds");
		return Names[index];
	}

	/// Get named token for mapper															
	///	@param index - the index to get													
	///	@return the token																		
	constexpr LiteralText Mapper::GetToken(pcptr index) {
		if (index >= pcptr(Enum::Counter))
			throw Except::BadAccess("Mapper out of bounds");
		return Names[index];
	}

	/// Get named token for refresh rate													
	///	@param index - the index to get													
	///	@return the token																		
	constexpr LiteralText RefreshRate::GetToken(pcptr index) {
		if (index >= pcptr(Enum::Counter))
			throw Except::BadAccess("Refresh rate out of bounds");
		return Names[index];
	}

	/// Check if refresh rate is considered a uniform									
	///	@return true if rate is considered a uniform									
	constexpr bool RefreshRate::IsUniform() const noexcept {
		return mState >= UniformBegin && mState < UniformEnd;
	}

	/// Check if refresh rate is considered a static uniform							
	///	@return true if rate is considered a static uniform						
	constexpr bool RefreshRate::IsStaticUniform() const noexcept {
		return mState >= StaticUniformBegin && mState < StaticUniformEnd;
	}

	/// Check if refresh rate is considered a dynamic uniform						
	///	@return true if rate is considered a dynamic uniform						
	constexpr bool RefreshRate::IsDynamicUniform() const noexcept {
		return mState >= DynamicUniformBegin && mState < DynamicUniformEnd;
	}

	/// Check if refresh rate is considered a vertex attribute						
	///	@return true if rate is considered a vertex attribute						
	constexpr bool RefreshRate::IsAttribute() const noexcept {
		return mState == RRate::PerVertex;
	}

	/// Check if refresh rate is considered an input									
	///	@return true if rate is considered a vertex attribute						
	constexpr bool RefreshRate::IsInput() const noexcept {
		return mState >= InputBegin && mState < InputEnd;
	}

	/// Get uniform buffer index																
	///	@return uniform buffer index														
	constexpr pcptr RefreshRate::GetInputIndex() const {
		SAFETY(if(!IsInput()) throw Except::Content());
		return mState - InputBegin;
	}

	/// Get static uniform buffer index														
	///	@return uniform buffer index														
	constexpr pcptr RefreshRate::GetStaticUniformIndex() const {
		SAFETY(if(!IsStaticUniform()) throw Except::Content());
		return mState - StaticUniformBegin;
	}

	/// Get dynamic uniform buffer index													
	///	@return uniform buffer index														
	constexpr pcptr RefreshRate::GetDynamicUniformIndex() const {
		SAFETY(if(!IsDynamicUniform()) throw Except::Content());
		return mState - DynamicUniformBegin;
	}

	/// Check if refresh rate corresponds to a shader stage							
	///	@return true if rate corresponds to a shader stage							
	constexpr bool RefreshRate::IsShaderStage() const noexcept {
		return mState >= StagesBegin && mState < StagesEnd;
	}

	/// Get shader stage index																	
	///	@return shader stage index															
	constexpr ShaderStage::Enum RefreshRate::GetStageIndex() const {
		if (!IsShaderStage())
			return ShaderStage::Counter;
		return ShaderStage::Enum(mState - StagesBegin);
	}

	///																								
	inline bool VertexView::operator == (const VertexView& other) const noexcept {
		return mPCount == other.mPCount && mPStart == other.mPStart 
			&& mICount == other.mICount && mIStart == other.mIStart 
			&& mPrimitive == other.mPrimitive;
	}

	///																								
	constexpr PixelView::PixelView(pcptr w, pcptr h, pcptr d, pcptr f,
		DMeta format, bool reversed) noexcept
		: mWidth(w)
		, mHeight(h)
		, mDepth(d)
		, mFrames(f)
		, mFormat(format)
		, mReverseFormat(reversed) {}

	inline bool PixelView::operator == (const PixelView& other) const noexcept {
		return mWidth == other.mWidth
			&& mHeight == other.mHeight
			&& mDepth == other.mDepth
			&& mFrames == other.mFrames
			&& mFormat == other.mFormat
			&& mReverseFormat == other.mReverseFormat;
	}

	/// Count the number of pixels															
	constexpr pcptr PixelView::CountPixels() const noexcept {
		return mWidth * mHeight * mDepth * mFrames;
	}

	/// Count the number of dimensions														
	constexpr pcptr PixelView::CountDimensions() const noexcept {
		return (mWidth > 1 ? 1 : 0) + (mHeight > 1 ? 1 : 0)
			+ (mDepth > 1 ? 1 : 0) + (mFrames > 1 ? 1 : 0);
	}

	/// Get pixel byte size																		
	inline pcptr PixelView::GetPixelSize() const noexcept {
		return mFormat->GetStride();
	}

	/// Count number of bytes																	
	inline pcptr PixelView::CountBytes() const noexcept {
		return CountPixels() * GetPixelSize();
	}

	/// Count number of color components													
	inline pcptr PixelView::CountComponents() const noexcept {
		return CountPixels() * mFormat->GetMemberList().size();
	}


	/// Get the content descriptor 															
	inline const Construct& AContent::GetConstruct() const noexcept {
		return mConstruct;
	}

	inline Construct& AContent::GetConstruct() noexcept {
		return mConstruct;
	}

	/// Get the generated data 																
	inline const TAny<Trait>& AContent::GetDataList() const noexcept {
		return mData;
	}

	inline TAny<Trait>& AContent::GetDataList() noexcept {
		return mData;
	}

	/// Get the generated data 																
	inline const TAny<Trait>& AContent::GetDataRangeList() const noexcept {
		return mDataRange;
	}

	inline TAny<Trait>& AContent::GetDataRangeList() noexcept {
		return mDataRange;
	}

	/// Check if content was generated														
	inline bool AContent::IsGenerated() const noexcept {
		return mGenerated;
	}

	/// Check if content was explicitly defined by providing a constructor		
	inline bool AContent::IsExplicitlyDefined() const noexcept {
		return mExplicitlyDefined;
	}

	/// Get generated data with specific tag and interpret it as a type			
	///	@param index - the element to get												
	///	@return a reference to the requested data										
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	decltype(auto) AContent::GetDataAs(const Index& index) {
		auto found = GetData(TRAIT::Reflect());
		if (!found) {
			throw Except::BadAccess(pcLogSelfError
				<< "No such data was found: " << TRAIT::ID);
		}

		return found->template As<DATA>(index);
	}

	/// Get generated data with specific tag and interpret it as a type (const)
	///	@param index - the element to get												
	///	@return a reference to the requested data										
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	decltype(auto) AContent::GetDataAs(const Index& index) const {
		return const_cast<ME*>(this)->GetDataAs<TRAIT, DATA>(index);
	}

	/// Get generated data with specific tag and interpret it as a type			
	///	@param index - the element to get												
	///	@return a reference to the requested data										
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	decltype(auto) AContent::GetDataRangeAs() {
		auto found = GetDataRange(TRAIT::Reflect());
		if (!found) {
			throw Except::BadAccess(pcLogSelfError
				<< "No such data was found: " << TRAIT::ID);
		}

		return found->template As<DATA>();
	}

	/// Get generated data with specific tag and interpret it as a type (const)
	///	@param index - the element to get												
	///	@return a reference to the requested data										
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	decltype(auto) AContent::GetDataRangeAs() const {
		return const_cast<ME*>(this)->GetDataRangeAs<TRAIT, DATA>();
	}

	/// Get a trait value from constructor, attempting a conversion				
	/// Throws if no trait available, or conversion failed							
	///	@param index - the offset															
	///	@return the value																		
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	NOD() DATA AContent::GetTraitValue(const pcptr& index) const {
		auto found = mConstruct.Get<TRAIT>(index);
		if (!found) {
			throw Except::MissingTrait(pcLogSelfError
				<< "Missing trait: " << TRAIT::ID);
		}

		return found->template AsCast<DATA>();
	}

	/// Set a trait value																		
	///	@param output - value to be written												
	///	@param index - the offset															
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
	void AContent::SetTraitValue(DATA&& value, const pcptr& index) {
		mConstruct.Set(Trait::From<TRAIT>(pcForward<DATA>(value)), index);
	}

	/// Get generated data block with specific tag										
	///	@param offset - optional offset for selecting multiple traits			
	///	@return a reference to the requested data										
	template<RTTI::ReflectedTrait TRAIT>
	Trait* AContent::GetData(pcptr offset) {
		return GetData(TRAIT::Reflect(), offset);
	}

	/// Get generated data block with specific tag (const)							
	///	@param offset - optional offset for selecting multiple traits			
	///	@return a reference to the requested data										
	template<RTTI::ReflectedTrait TRAIT>
	const Trait* AContent::GetData(pcptr offset) const {
		return const_cast<ME*>(this)->GetData<TRAIT>(offset);
	}

	/// Get generated data block range with specific tag								
	///	@param offset - optional offset for selecting multiple traits			
	///	@return a reference to the requested data range								
	template<RTTI::ReflectedTrait TRAIT>
	Trait* AContent::GetDataRange(pcptr offset) {
		return GetDataRange(TRAIT::Reflect(), offset);
	}

	/// Get generated data block range with specific tag (const)					
	///	@param offset - optional offset for selecting multiple traits			
	///	@return a reference to the requested data range								
	template<RTTI::ReflectedTrait TRAIT>
	const Trait* AContent::GetDataRange(pcptr offset) const {
		return const_cast<ME*>(this)->GetDataRange<TRAIT>(offset);
	}

	/// Save data without range																
	///	@param content - the data to save												
	template<RTTI::ReflectedTrait TRAIT>
	void AContent::SaveData(Any&& content) {
		SaveData(TRAIT::ID, pcForward<Any>(content), Any {});
	}

	/// Save data with range																	
	///	@param content - the data to save												
	///	@param min - the minimum value													
	///	@param max - the maximum value													
	template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData RANGE>
	void AContent::SaveData(Any&& content, const RANGE& min, const RANGE& max) {
		if constexpr (ComplexNumber<RANGE>)
			SaveData(TRAIT::ID, pcForward<Any>(content), Any {TComplexRange<RANGE>{min, max}});
		else
			SaveData(TRAIT::ID, pcForward<Any>(content), Any {TRange<RANGE>{min, max}});
	}

	/// Get the texture mapper																	
	///	@return the mapper																	
	inline Mapper AGeometry::GetTextureMapper() const noexcept {
		return mTextureMapper;
	}

	/// Get the tesselation factor															
	///	@return the tesselation																
	inline pcptr AGeometry::GetTesselation() const noexcept {
		return mTesselation;
	}

	/// Get the bilaterality factor															
	///	@return true if geometry is two-sided											
	inline bool AGeometry::GetBilaterality() const noexcept {
		return mView.mBilateral;
	}

	/// Get the topology																			
	///	@return the topology of the geometry											
	inline DMeta AGeometry::GetTopology() const noexcept {
		return mView.mPrimitive;
	}

	/// Set geometry topology via a static topology type								
	template<RTTI::ReflectedData T>
	void AGeometry::SetTopology() {
		SetTopology(DataID::Reflect<T>());
	}

	/// Check if geometry has similar topology via a static topology type		
	///	@return true if topology matches													
	template<RTTI::ReflectedData T>
	bool AGeometry::CheckTopology() const {
		return GetTopology()->InterpretsAs<T>();
	}

	/// Check if geometry has specific topology via a static topology type		
	///	@return true if topology matches													
	template<RTTI::ReflectedData T>
	bool AGeometry::IsTopology() const {
		return GetTopology()->Is<T>();
	}

} // namespace PCFW
