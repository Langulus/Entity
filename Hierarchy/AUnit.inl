namespace PCFW
{

	/// Construct produced																		
	///	@param producer - the producer to set											
	template<Dense T>
	TProducedFrom<T>::TProducedFrom(T* producer) noexcept
		: mProducer {producer} {}

	/// Move-assign a new producer															
	///	@param not used																		
	template<Dense T>
	TProducedFrom<T>& TProducedFrom<T>::operator = (TProducedFrom<T>&&) noexcept {
		// You can't move-assign a new producer									
		return *this;
	}

	/// Get a value from inside a trait by scanning owners and other units		
	///	@param value - [out] value to set if trait was found						
	///	@return true if anything was written to value								
	template<RTTI::ReflectedTrait T, RTTI::ReflectedData D>
	bool AUnit::SeekValue(D& value) const {
		return SeekValue<D>(T::Reflect(), value);
	}

} // namespace PCFW
