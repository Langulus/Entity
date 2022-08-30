#pragma once
#include "../IntegrateFramework.hpp"
#include "../Hierarchy/Entity.hpp"
#include "../Hierarchy/CRuntime.hpp"
#include "../Flow/CFlow.hpp"
#include "../Containers/TMap.hpp"
#include "../Containers/TFactory.hpp"
#include "../IncludeModules.hpp"

namespace PCFW
{

	LANGULUS_MODULE(FRAMEWORK) bool pcFrameworkInit(pcptr = 0);
	LANGULUS_MODULE(FRAMEWORK) bool pcInitFrameworkMeta();

} // namespace PCFW