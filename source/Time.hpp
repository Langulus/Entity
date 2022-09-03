#pragma once
#include "Common.hpp"
#include <chrono>

namespace Langulus::Entity
{

	///																								
	///	A steady clock used to aquire TimePoint(s)									
	///																								
	using SteadyClock = ::std::chrono::steady_clock;


	///																								
	///	A time point																			
	///																								
	class TimePoint : public SteadyClock::time_point {
	public:
		using Base = SteadyClock::time_point;
		using Base::time_point;

		constexpr TimePoint() noexcept
			: time_point {min()} {}

		constexpr explicit operator bool() const noexcept {
			return *this != min();
		}

		NOD() constexpr Hash GetHash() const noexcept {
			using Representation = typename Base::rep;
			static_assert(sizeof(Representation) == sizeof(TimePoint),
				"Size mismatch");
			return HashNumber(reinterpret_cast<const Representation&>(*this));
		}
	};


	///																								
	///	A time duration (difference between two time points)						
	///																								
	class Time : public SteadyClock::duration {
	public:
		using Base = SteadyClock::duration;
		using Base::duration;

		constexpr Time() noexcept
			: duration {zero()} {}

		constexpr explicit operator bool() const noexcept {
			return *this != zero();
		}

		NOD() constexpr Hash GetHash() const noexcept {
			using Representation = typename Base::rep;
			static_assert(sizeof(Representation) == sizeof(Time),
				"Size mismatch");
			return HashNumber(reinterpret_cast<const Representation&>(*this));
		}
	};

} // namespace Langulus::Entity