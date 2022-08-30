#pragma once
#include <PCFW.Memory.hpp>
#include <PCFW.Flow.hpp>
#include <PCFW.Math.hpp>

#ifdef PC_EXPORT_PCFW
	#define LANGULUS_API_FOR_FRAMEWORK() LANGULUS_EXPORT()
#else
	#define LANGULUS_API_FOR_FRAMEWORK() LANGULUS_IMPORT()
#endif

namespace PCFW
{
	using namespace ::PCFW::Memory;
	using namespace ::PCFW::Math;
	using namespace ::PCFW::Flow;
}