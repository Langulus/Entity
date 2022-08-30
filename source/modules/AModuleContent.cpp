#include "AModuleContent.hpp"

#define PC_VERBOSE_GEOMETRY(a) //pcLogSelfVerbose << a

namespace PCFW
{

	/// Get the constructor hash																
	///	@return the hash of the constructor												
	NOD() Hash AContent::GetHash() const {
		return mConstruct.GetHash();
	}

	/// Contents are always compared using only the construct						
	///	@param other - the content to compare against								
	bool AContent::operator == (const AContent& other) const {
		return mConstruct == other.mConstruct;
	}

	bool AContent::operator != (const AContent& other) const {
		return mConstruct != other.mConstruct;
	}

	/// Uninitialize content																	
	void AContent::Uninitialize() {
		ClassInvalidate();
		mData.Clear();
		mDataRange.Clear();
		mConstruct.Clear();
		mGenerated = false;
		mExplicitlyDefined = false;
	}

	/// Get generated data block with specific tag										
	///	@param trait - the trait to search for											
	///	@param offset - optional offset for selecting multiple traits			
	///	@return a reference to the requested data										
	Trait* AContent::GetData(TMeta trait, pcptr offset) {
		for (auto& t : mData) {
			if (t.GetTraitMeta() == trait) {
				if (offset == 0)
					return &t;
				--offset;
			}
		}
		return nullptr;
	}

	const Trait* AContent::GetData(TMeta trait, pcptr offset) const {
		return const_cast<ME*>(this)->GetData(trait, offset);
	}

	/// Get generated data block with specific tag										
	///	@param trait - the trait to search for											
	///	@param offset - optional offset for selecting multiple traits			
	///	@return a reference to the requested data										
	Trait* AContent::GetDataRange(TMeta trait, pcptr offset) {
		for (auto& t : mDataRange) {
			if (t.GetTraitMeta() == trait) {
				if (offset == 0)
					return &t;
				--offset;
			}
		}
		return nullptr;
	}

	const Trait* AContent::GetDataRange(TMeta trait, pcptr offset) const {
		return const_cast<ME*>(this)->GetDataRange(trait, offset);
	}

	/// Save into raw content data by moving a memory block							
	///	@param trait - the type of content to save									
	///	@param content - the content to move to the raw data section			
	///	@param range - the content range to move to the data range section	
	void AContent::SaveData(const TraitID trait, Any&& content, Any&& range) {
		mData << Trait {trait.GetMeta(), pcForward<Any>(content)};
		mDataRange << Trait {trait.GetMeta(), pcForward<Any>(range)};
	}

	/// Set topology manually																	
	///	@param topologyMeta - the topology to set										
	void AGeometry::SetTopology(DMeta topologyMeta) {
		SAFETY(if (!topologyMeta->InterpretsAs<ATopology>())
			throw Except::Content(pcLogSelfError 
				<< "Bad geometry topology provided: "
				<< topologyMeta->GetToken() << " - it must inherit ATopology")
		);

		mConstruct.Set(Trait::From<Traits::Topology>(topologyMeta->GetID()));
		mView.mPrimitive = topologyMeta;
		mExplicitlyDefined = true;
		PC_VERBOSE_GEOMETRY("Topology is now " << topologyMeta->GetToken());
	}

	/// Set texture mapper																		
	///	@param mapper - the mapper to set												
	void AGeometry::SetTextureMapper(Mapper mapper) {
		mConstruct.Set(Trait::From<Traits::TextureMapper>(mapper));
		mTextureMapper = mapper;
		mExplicitlyDefined = true;
		PC_VERBOSE_GEOMETRY("Mapper is now " << mapper);
	}

	/// Set tesselation manually																
	///	@param tesselation - the tesselation to set									
	void AGeometry::SetTesselation(pcptr tesselation) {
		mConstruct.Set(Trait::From<Traits::Tesselation>(tesselation));
		mTesselation = tesselation;
		mExplicitlyDefined = true;
		PC_VERBOSE_GEOMETRY("Tesselation is now " << tesselation);
	}

	/// Set bilaterality manually																
	///	@param enabled - toggle																
	void AGeometry::SetBilaterality(bool enabled) {
		mConstruct.Set(Trait::From<Traits::Bilateral>(enabled));
		mView.mBilateral = enabled;
		mExplicitlyDefined = true;
		PC_VERBOSE_GEOMETRY("Bilaterality is now " << enabled);
	}

	/// Decay a primitive buffer view to vertices										
	///	@return the decayed vertex view													
	VertexView VertexView::Decay() const {
		if (!mPCount || !mPrimitive)
			throw Except::Content(pcLogFuncError << "Bad vertex view");

		VertexView result = *this;
		if (mPrimitive->InterpretsAs<APoint>())
			return result;

		// PCount corresponds to the number of points in these cases		
		if (  mPrimitive->InterpretsAs<ATriangleStrip>()
			|| mPrimitive->InterpretsAs<ATriangleFan>()
			|| mPrimitive->InterpretsAs<ALineStrip>()
			|| mPrimitive->InterpretsAs<ALineLoop>()) {
			result.mPrimitive = DataID::Reflect<APoint>();
			return result;
		}

		if (mPrimitive->InterpretsAs<ATriangle>()) {
			// Decay triangles to points												
			result.mPCount *= 3;
			result.mPrimitive = DataID::Reflect<APoint>();
		}
		else if (mPrimitive->InterpretsAs<ALine>()) {
			// Decay lines to points													
			result.mPCount *= 2;
			result.mPrimitive = DataID::Reflect<APoint>();
		}
		else {
			throw Except::Content(pcLogFuncError
				<< "Bad primitive type: " << mPrimitive->GetToken());
		}

		return result;
	}

} // namespace PCFW