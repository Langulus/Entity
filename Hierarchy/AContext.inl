namespace PCFW
{

	/// Context manual construction															
	///	@param classid - class type overrider											
	///	@param state - the initial context state										
	constexpr AContext::AContext(DMeta classid, const CState& state) noexcept
		: mClass{ classid }
		, mClassState{ state } { }

	/// Check if context interprets as a static type									
	///	@tparam T - the type to check for												
	///	@return true if this context can be dynamically interpreted as T		
	template<class T>
	bool AContext::ClassInterpretsAs() const {
		return ClassInterpretsAs(DataID::Reflect<T>());
	}

	/// Check if context is an exact static type											
	///	@tparam T - the type to check for												
	///	@return true if this context can be dynamically interpreted as T		
	template<class T>
	bool AContext::ClassIs() const {
		return ClassIs(DataID::Of<T>);
	}

	/// Get the class meta																		
	///	@return the meta data for the class type										
	inline DMeta AContext::ClassMeta() const noexcept {
		return mClass;
	}
	
	/// Get the class name token																
	///	@return the token																		
	inline const Text AContext::ClassToken() const {
		return mClass->GetToken();
	}

	/// Get the class state																		
	///	@return the class state																
	inline CState AContext::ClassState() const noexcept {
		return mClassState;
	}

	/// Get modifiable class state															
	///	@return the modifiable class state												
	inline CState& AContext::ClassState() noexcept {
		return mClassState;
	}

	/// Check if context interprets as a type												
	///	@param type - the type to check for												
	///	@return true if this context can be dynamically interpreted to type	
	inline bool AContext::ClassInterpretsAs(DMeta type) const {
		return !mClass || mClass->InterpretsAs(type);
	}

	/// Check if context is an exact type													
	///	@param type - the type to check for												
	///	@return true if this context can be dynamically interpreted to type	
	inline bool AContext::ClassIs(DataID type) const {
		return mClass && mClass->Is(type);
	}

	/// Check if this context is enabled and valid										
	///	@return true if context is enabled and valid									
	inline bool AContext::IsClassIrrelevant() const noexcept {
		return IsClassDisabled() || IsClassInvalid();
	}

	/// Check if this context is disabled													
	///	@return true if context is disabled												
	inline bool AContext::IsClassDisabled() const noexcept {
		return mClassState & CState::Disabled;
	}

	/// Check if this context is enabled													
	///	@return true if context is enabled												
	inline bool AContext::IsClassEnabled() const noexcept {
		return mClassState % CState::Disabled;
	}

	/// Check if this context is invalid													
	///	@return true if context is invalid												
	inline bool AContext::IsClassInvalid() const noexcept {
		return mClassState & CState::Invalid;
	}

	/// Check if this context is valid														
	///	@return true if context is valid													
	inline bool AContext::IsClassValid() const noexcept {
		return mClassState % CState::Invalid;
	}

	/// Make this context valid																
	inline void AContext::ClassValidate() noexcept {
		mClassState -= CState::Invalid;
	}

	/// Make this context invalid																
	inline void AContext::ClassInvalidate() noexcept {
		mClassState += CState::Invalid;
	}

	/// Enable this context																		
	inline void AContext::ClassEnable() noexcept {
		mClassState -= CState::Disabled;
	}

	/// Disable this context																	
	inline void AContext::ClassDisable() noexcept {
		mClassState += CState::Disabled;
	}

} // namespace PCFW
