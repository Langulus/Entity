#pragma once
#include "../Hierarchy/AModule.hpp"

namespace PCFW
{

	///																								
	///	ABSTRACT LOGGING MODULE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleLogger : public AModule {
		REFLECT(AModuleLogger);
	public:
		using AModule::AModule;

		virtual void Log(const Debug&) = 0;
		virtual void SetColor(const rgba&) = 0;
		virtual void StartTest(const Debug& label, const pcptr iteration, const pcptr elements) = 0;
		virtual pcptr EndTest(bool post = false) = 0;
	};

} // namespace PCFW