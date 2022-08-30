#pragma once
#include "../IntegrateFramework.hpp"

LANGULUS_DECLARE_EXCEPTION(BadFlow);

namespace PCFW
{

	constexpr auto noPriority = Charge::MaxPriority;

	class MissingPoint;
	using Futures = TAny<MissingPoint*>;
	using Pasts = TAny<MissingPoint*>;
	using Filter = TAny<DataID>;


	///																								
	///	A missing point inside a flow														
	///																								
	class LANGULUS_MODULE(FRAMEWORK) MissingPoint {
		REFLECT_MANUALLY(MissingPoint) {
			return RTTI::ReflectData::From<ME>(
				"MissingPoint,MissingPointPtr,MissingPointConstPtr", "A missing point inside a flow");
		}

	public:
		NOD() bool IsFork() const noexcept;
		NOD() const Filter& GetFilter() const SAFE_NOEXCEPT();
		NOD() bool Accepts(const Block&) const SAFE_NOEXCEPT();
		NOD() const Block& GetContent() const SAFE_NOEXCEPT();
		NOD() bool HasRelevantContent() const SAFE_NOEXCEPT();

		void AddContent(Any&);
		void Collapse();

		Futures& AddBranch();

		void Dump() const;

		template<bool ATTEMPT, bool CLONE>
		bool FilterAndInsert(Any&, const TFunctor<Pasts(Any&)>&);
		
		template<bool ATTEMPT, bool CLONE>
		static bool Insert(MissingPoint&, Any&, Any&, const TFunctor<Pasts(Any&)>&);

	public:
		real mPriority;
		Ptr<Any> mPack;
		Construct* mChargeFor {};
		pcptr mChanges = 0;
	};


	///																								
	///	A fork - place where the flow separates in several branches				
	///																								
	struct Fork {
		REFLECT_MANUALLY(Fork) {
			return RTTI::ReflectData::From<ME>(
				"Fork,ForkPtr,ForkConstPtr", "A flow fork");
		}

	public:
		Ptr<Any> mRoot;
		Any mIdentity;
		bool mDedicatedIdentity = false;
		TAny<Futures> mBranches;
	};

} // namespace PCFW

#include "MissingFlow.inl"