#pragma once
#include "../Hierarchy/AModule.hpp"
#include "../Hierarchy/AUnit.hpp"

namespace PCFW
{

	///																								
	///	ABSTRACT INPUT MODULE INTERFACE													
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleInput : public AModule {
		REFLECT(AModuleInput);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT INPUT MODULE UNIT															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitInput : public AUnit {
		REFLECT(AUnitInput);
		using AUnit::AUnit;
	};


	///																								
	///	EVENT STATE																				
	///																								
	class LANGULUS_MODULE(FRAMEWORK) EventState : public TNamed<EventState, pcu8> {
		REFLECT(EventState);
	public:
		enum Enum : pcu8 {
			Any = 0,					// Any event state								
			Activated = 1,			// Event was activated							
			Deactivated = 2,		// Event was deactivated						
			WasActive = 4,			// Event persisted in active state			
			Disabled = 8,			// Event disabled									
			Locked = 16,			// Event locked									

			// OS specific																	
			DoubleClick = 32,		// The key has been double-tapped			
			Repeat = 64,			// The key was held down until repeated	

			// Shortcuts																	
			Released = WasActive | Deactivated,
		};

		/// Get named token																		
		///	@param index - the index to get												
		///	@return the token																	
		static constexpr LiteralText GetToken(pcptr index) {
			constexpr LiteralText names[] = {
				"esAny",
				"esActivated",
				"esDeactivated",
				"esWasActive",
				"esDisabled",
				"esLocked",
				"esDoubleClick",
				"esRepeat"
			};

			if (index >= sizeof(names) / sizeof(LiteralText))
				throw Except::BadAccess("Named array out of bounds");
			return names[index];
		}

		using TNamed::TNamed;

		constexpr EventState(EventState::Enum initial)
			: TNamed(initial) {}
	};

	using EState = EventState;


	///																								
	/// An event																					
	///																								
	class LANGULUS_MODULE(FRAMEWORK) Event {
		REFLECT(Event);
	public:
		static constexpr LiteralText DefaultToken = "Event";

		Event(DMeta e = {}, EState s = {}) noexcept
			: mEvent(e), mState(s) {}

		inline bool operator == (const Event& other) const noexcept {
			return mEvent == other.mEvent;
		}
		inline bool operator != (const Event& other) const noexcept {
			return mEvent != other.mEvent;
		}

		/// Serialize event to GASM															
		explicit operator GASM() const {
			GASM result;
			result += mEvent ? mEvent->GetToken() : DefaultToken;
			result += GASM::OpenScope;
			if (mState)
				result += mState;
			result += GASM::CloseScope;
			return result;
		}

	public:
		DMeta mEvent = nullptr;
		EState mState;
	};

} // namespace PCFW

LANGULUS_DECLARE_TRAIT(MousePosition, "Mouse position trait");
LANGULUS_DECLARE_TRAIT(MouseScroll, "Mouse scroll trait");

LANGULUS_DECLARE_CONSTANT(AnyES, EState(EState::Any), "Any event state");
LANGULUS_DECLARE_CONSTANT(ActivatedES, EState(EState::Activated), "Activated event state");
LANGULUS_DECLARE_CONSTANT(DeactivatedES, EState(EState::Deactivated), "Deactivated event state");
LANGULUS_DECLARE_CONSTANT(WasActiveES, EState(EState::WasActive), "Was-active event state");
LANGULUS_DECLARE_CONSTANT(DisabledES, EState(EState::Disabled), "Disabled event state");
LANGULUS_DECLARE_CONSTANT(LockedES, EState(EState::Locked), "Locked event state");
LANGULUS_DECLARE_CONSTANT(DoubleClickES, EState(EState::DoubleClick), "Double-click event state");
LANGULUS_DECLARE_CONSTANT(RepeatES, EState(EState::Repeat), "Repeat event state");
LANGULUS_DECLARE_CONSTANT(ReleasedES, EState(EState::Released), "Released event state");

/// Use this macro to declare events conveniently										
///	@param a - the name of the event														
///	@param info - information literal													
#define LANGULUS_DECLARE_EVENT(a, info) \
	namespace PCFW::Events { \
		class a : public Event { \
		public: \
			static constexpr DataID ID { RTTI::pcGetTypeID<a>() }; \
			static constexpr auto Switch = ID.GetHash().GetValue(); \
			static inline auto Reflect() noexcept { \
				return DataID::Reflect<a>(); \
			} \
			REFLECT_MANUALLY(a) { \
				auto result = RTTI::ReflectData::From<a>( \
					"Event" #a ",Event" #a "Ptr,Event" #a "ConstPtr", info); \
				result.SetBases<a>(REFLECT_BASE(Event)); \
				return result; \
			} \
		public: \
			a (EState s = {}) : Event(Reflect(), s) {} \
		}; \
	}

/// Use this macro to declare keys conveniently											
/// Keys are no different than events, except for their nomenclature				
///	@param a - the name of the key														
///	@param info - information literal													
#define LANGULUS_DECLARE_KEY(a, info) \
	namespace PCFW::Keys { \
		class a : public Event { \
		public: \
			static constexpr DataID ID { RTTI::pcGetTypeID<a>() }; \
			static constexpr auto Switch = ID.GetHash().GetValue(); \
			static inline auto Reflect() noexcept { \
				return DataID::Reflect<a>(); \
			} \
			REFLECT_MANUALLY(a) { \
				auto result = RTTI::ReflectData::From<a>( \
					"Key" #a ",Key" #a "Ptr,Key" #a "ConstPtr", info); \
				result.SetBases<a>(REFLECT_BASE(Event)); \
				return result; \
			} \
		public: \
			a (EState s = {}) : Event(Reflect(), s) {} \
		}; \
	}


/// Declare some abstract built-in events													
LANGULUS_DECLARE_EVENT(Ok, "Success event");
LANGULUS_DECLARE_EVENT(Verbose, "Verbose event");
LANGULUS_DECLARE_EVENT(Info, "Info event");
LANGULUS_DECLARE_EVENT(Error, "Error event");
LANGULUS_DECLARE_EVENT(Warning, "Warning event");
LANGULUS_DECLARE_EVENT(Message, "Message event");
LANGULUS_DECLARE_EVENT(Special, "Special event");
LANGULUS_DECLARE_EVENT(Flow, "Flow event");
LANGULUS_DECLARE_EVENT(Input, "Input event");
LANGULUS_DECLARE_EVENT(TCP, "TCP event");
LANGULUS_DECLARE_EVENT(UDP, "UDP event");
LANGULUS_DECLARE_EVENT(OS, "OS event");
LANGULUS_DECLARE_EVENT(Choice, "Choice event");

/// Declare some abstract input events														
LANGULUS_DECLARE_KEY(Space, "Spacebar");
LANGULUS_DECLARE_KEY(Quote, "Quote");
LANGULUS_DECLARE_KEY(Comma, "Comma");
LANGULUS_DECLARE_KEY(Minus, "Minus sign");
LANGULUS_DECLARE_KEY(Period, "Period");
LANGULUS_DECLARE_KEY(Slash, "Slash");

LANGULUS_DECLARE_KEY(Key0, "0 keyboard key");
LANGULUS_DECLARE_KEY(Key1, "1 keyboard key");
LANGULUS_DECLARE_KEY(Key2, "2 keyboard key");
LANGULUS_DECLARE_KEY(Key3, "3 keyboard key");
LANGULUS_DECLARE_KEY(Key4, "4 keyboard key");
LANGULUS_DECLARE_KEY(Key5, "5 keyboard key");
LANGULUS_DECLARE_KEY(Key6, "6 keyboard key");
LANGULUS_DECLARE_KEY(Key7, "7 keyboard key");
LANGULUS_DECLARE_KEY(Key8, "8 keyboard key");
LANGULUS_DECLARE_KEY(Key9, "9 keyboard key");

LANGULUS_DECLARE_KEY(Semicolon, "Semicolon");
LANGULUS_DECLARE_KEY(NumpadEqual, "Equal at numpad");

LANGULUS_DECLARE_KEY(A, "A keyboard key");
LANGULUS_DECLARE_KEY(B, "B keyboard key");
LANGULUS_DECLARE_KEY(C, "C keyboard key");
LANGULUS_DECLARE_KEY(D, "D keyboard key");
LANGULUS_DECLARE_KEY(E, "E keyboard key");
LANGULUS_DECLARE_KEY(F, "F keyboard key");
LANGULUS_DECLARE_KEY(G, "G keyboard key");
LANGULUS_DECLARE_KEY(H, "H keyboard key");
LANGULUS_DECLARE_KEY(I, "I keyboard key");
LANGULUS_DECLARE_KEY(J, "J keyboard key");
LANGULUS_DECLARE_KEY(K, "K keyboard key");
LANGULUS_DECLARE_KEY(L, "L keyboard key");
LANGULUS_DECLARE_KEY(M, "M keyboard key");
LANGULUS_DECLARE_KEY(N, "N keyboard key");
LANGULUS_DECLARE_KEY(O, "O keyboard key");
LANGULUS_DECLARE_KEY(P, "P keyboard key");
LANGULUS_DECLARE_KEY(Q, "Q keyboard key");
LANGULUS_DECLARE_KEY(R, "R keyboard key");
LANGULUS_DECLARE_KEY(S, "S keyboard key");
LANGULUS_DECLARE_KEY(T, "T keyboard key");
LANGULUS_DECLARE_KEY(U, "U keyboard key");
LANGULUS_DECLARE_KEY(V, "V keyboard key");
LANGULUS_DECLARE_KEY(W, "W keyboard key");
LANGULUS_DECLARE_KEY(X, "X keyboard key");
LANGULUS_DECLARE_KEY(Y, "Y keyboard key");
LANGULUS_DECLARE_KEY(Z, "Z keyboard key");

LANGULUS_DECLARE_KEY(LeftBracket, "Left bracket");
LANGULUS_DECLARE_KEY(Hack, "\\ symbol");
LANGULUS_DECLARE_KEY(RightBracket, "Right bracket");
LANGULUS_DECLARE_KEY(Tilde, "~ symbol");
LANGULUS_DECLARE_KEY(Escape, "Escape");
LANGULUS_DECLARE_KEY(Enter, "Enter");
LANGULUS_DECLARE_KEY(Tab, "Tab");
LANGULUS_DECLARE_KEY(Back, "Backspace");
LANGULUS_DECLARE_KEY(Insert, "Insert");
LANGULUS_DECLARE_KEY(Delete, "Delete");
LANGULUS_DECLARE_KEY(Right, "Right arrow");
LANGULUS_DECLARE_KEY(Left, "Left arrow");
LANGULUS_DECLARE_KEY(Down, "Down arrow");
LANGULUS_DECLARE_KEY(Up, "Up arrow");
LANGULUS_DECLARE_KEY(PageUp, "Page up");
LANGULUS_DECLARE_KEY(PageDown, "Page down");
LANGULUS_DECLARE_KEY(Home, "Home");
LANGULUS_DECLARE_KEY(End, "End");
LANGULUS_DECLARE_KEY(CapsLock, "Caps Lock");
LANGULUS_DECLARE_KEY(ScrollLock, "Scroll Lock");
LANGULUS_DECLARE_KEY(NumLock, "Numpad Lock");
LANGULUS_DECLARE_KEY(Print, "Print");
LANGULUS_DECLARE_KEY(Pause, "Pause");

LANGULUS_DECLARE_KEY(F1, "F1 keyboard key");
LANGULUS_DECLARE_KEY(F2, "F2 keyboard key");
LANGULUS_DECLARE_KEY(F3, "F3 keyboard key");
LANGULUS_DECLARE_KEY(F4, "F4 keyboard key");
LANGULUS_DECLARE_KEY(F5, "F5 keyboard key");
LANGULUS_DECLARE_KEY(F6, "F6 keyboard key");
LANGULUS_DECLARE_KEY(F7, "F7 keyboard key");
LANGULUS_DECLARE_KEY(F8, "F8 keyboard key");
LANGULUS_DECLARE_KEY(F9, "F9 keyboard key");
LANGULUS_DECLARE_KEY(F10, "F10 keyboard key");
LANGULUS_DECLARE_KEY(F11, "F11 keyboard key");
LANGULUS_DECLARE_KEY(F12, "F12 keyboard key");
LANGULUS_DECLARE_KEY(F13, "F13 keyboard key");
LANGULUS_DECLARE_KEY(F14, "F14 keyboard key");
LANGULUS_DECLARE_KEY(F15, "F15 keyboard key");
LANGULUS_DECLARE_KEY(F16, "F16 keyboard key");
LANGULUS_DECLARE_KEY(F17, "F17 keyboard key");
LANGULUS_DECLARE_KEY(F18, "F18 keyboard key");
LANGULUS_DECLARE_KEY(F19, "F19 keyboard key");
LANGULUS_DECLARE_KEY(F20, "F20 keyboard key");
LANGULUS_DECLARE_KEY(F21, "F21 keyboard key");
LANGULUS_DECLARE_KEY(F22, "F22 keyboard key");
LANGULUS_DECLARE_KEY(F23, "F23 keyboard key");
LANGULUS_DECLARE_KEY(F24, "F24 keyboard key");

LANGULUS_DECLARE_KEY(Num0, "Numpad 0");
LANGULUS_DECLARE_KEY(Num1, "Numpad 1");
LANGULUS_DECLARE_KEY(Num2, "Numpad 2");
LANGULUS_DECLARE_KEY(Num3, "Numpad 3");
LANGULUS_DECLARE_KEY(Num4, "Numpad 4");
LANGULUS_DECLARE_KEY(Num5, "Numpad 5");
LANGULUS_DECLARE_KEY(Num6, "Numpad 6");
LANGULUS_DECLARE_KEY(Num7, "Numpad 7");
LANGULUS_DECLARE_KEY(Num8, "Numpad 8");
LANGULUS_DECLARE_KEY(Num9, "Numpad 9");

LANGULUS_DECLARE_KEY(NumDecimal, "Numpad decimal");
LANGULUS_DECLARE_KEY(NumDivide, "Numpad divide");
LANGULUS_DECLARE_KEY(NumMultiply, "Numpad multiply");
LANGULUS_DECLARE_KEY(NumSubtract, "Numpad subtract");
LANGULUS_DECLARE_KEY(NumAdd, "Numpad add");

LANGULUS_DECLARE_KEY(LShift, "Left shift");
LANGULUS_DECLARE_KEY(LControl, "Left control");
LANGULUS_DECLARE_KEY(LAlt, "Left alt");
LANGULUS_DECLARE_KEY(RShift, "Right shift");
LANGULUS_DECLARE_KEY(RControl, "Right control");
LANGULUS_DECLARE_KEY(RAlt, "Right alt");

LANGULUS_DECLARE_KEY(MouseLeft, "Left mouse button");
LANGULUS_DECLARE_KEY(MouseMiddle, "Middle mouse button");
LANGULUS_DECLARE_KEY(MouseRight, "Right mouse button");
LANGULUS_DECLARE_KEY(Mouse4, "Mouse button #4");
LANGULUS_DECLARE_KEY(Mouse5, "Mouse button #5");
LANGULUS_DECLARE_KEY(Mouse6, "Mouse button #6");
LANGULUS_DECLARE_KEY(Mouse7, "Mouse button #7");
LANGULUS_DECLARE_KEY(Mouse8, "Mouse button #8");

LANGULUS_DECLARE_KEY(Joystick1, "Joystick button #1");
LANGULUS_DECLARE_KEY(Joystick2, "Joystick button #2");
LANGULUS_DECLARE_KEY(Joystick3, "Joystick button #3");
LANGULUS_DECLARE_KEY(Joystick4, "Joystick button #4");
LANGULUS_DECLARE_KEY(Joystick5, "Joystick button #5");
LANGULUS_DECLARE_KEY(Joystick6, "Joystick button #6");
LANGULUS_DECLARE_KEY(Joystick7, "Joystick button #7");
LANGULUS_DECLARE_KEY(Joystick8, "Joystick button #8");
LANGULUS_DECLARE_KEY(Joystick9, "Joystick button #9");
LANGULUS_DECLARE_KEY(Joystick10, "Joystick button #10");
LANGULUS_DECLARE_KEY(Joystick11, "Joystick button #11");
LANGULUS_DECLARE_KEY(Joystick12, "Joystick button #12");
LANGULUS_DECLARE_KEY(Joystick13, "Joystick button #13");
LANGULUS_DECLARE_KEY(Joystick14, "Joystick button #14");
LANGULUS_DECLARE_KEY(Joystick15, "Joystick button #15");
LANGULUS_DECLARE_KEY(Joystick16, "Joystick button #16");

LANGULUS_DECLARE_KEY(MouseMoveHorizontal, "Horizontal mouse movement event");
LANGULUS_DECLARE_KEY(MouseMoveVertical, "Vertical mouse movement event");
LANGULUS_DECLARE_KEY(MouseScrollHorizontal, "Horizontal mouse scrolling event");
LANGULUS_DECLARE_KEY(MouseScrollVertical, "Vertical mouse scrolling event");
