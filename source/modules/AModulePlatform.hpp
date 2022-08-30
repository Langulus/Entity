#pragma once
#include "../AModule.hpp"	
#include "../AUnit.hpp"

namespace PCFW
{

	class ARenderer;


	///																								
	///	ABSTRACT PLATFORM MODULE INTERFACE												
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModulePlatform : public AModule {
		REFLECT(AModulePlatform);
		using AModule::AModule;

		virtual pcptr GetNumberOfActiveWindows() const = 0;
	};


	///																								
	///	ABSTRACT PLATFORM UNIT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitPlatform : public AUnit {
		REFLECT(AUnitPlatform);
		using AUnit::AUnit;
	};


	///																								
	///	ABSTRACT WINDOW INTERFACE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AWindow : public AUnitPlatform {
		REFLECT(AWindow);
		AWindow(DMeta);
		AWindow(AWindow&&) noexcept;
		AWindow& operator = (AWindow&&) noexcept;

		virtual void BindContext() = 0;
		NOD() virtual bool IsInFocus() const = 0;
		NOD() virtual bool IsMinimized() const = 0;
		NOD() virtual bool IsMouseOver() const = 0;
		NOD() virtual bool IsInteractable() const = 0;

		PC_GET(NativeWindowHandle);
		PC_GET(Size);
		PC_GET(Title);
		PC_GET(MousePosition);
		PC_GET(MouseScroll);
		PC_GET(TextInput);

	protected:
		// The renderer																	
		Ptr<ARenderer> mRenderer;
		// Window handle (OS dependent, likely out of jurisdiction)			
		Own<void*> mNativeWindowHandle;
		// The window size																
		vec2 mSize { 640, 480 };
		// The window title																
		Text mTitle { "<no title>" };
		// Mouse position gradient														
		vec2Grad mMousePosition;
		// Mouse scroll gradient														
		vec2Grad mMouseScroll;
		// Text input gets buffered here												
		Text mTextInput;
	};

} // namespace PCFW

LANGULUS_DECLARE_EXCEPTION(Platform);
