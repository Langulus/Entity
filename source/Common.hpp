#pragma once
#include <Langulus.Flow.hpp>
#include <chrono>

namespace Langulus::Entity
{

	using namespace ::Langulus::Anyness;
	using ::Langulus::Flow::Verb;
	using ::Langulus::Flow::Construct;
	using ::Langulus::Flow::Charge;
	using ::Langulus::Flow::SeekStyle;

	using TimePoint = ::std::chrono::steady_clock::time_point;
	using Time = ::std::chrono::steady_clock::duration;

} //namespace Langulus::Entity