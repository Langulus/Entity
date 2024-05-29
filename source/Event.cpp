///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include "Event.inl"


namespace Langulus
{

   /// Log an event                                                           
   Event::operator Text() const {
      if (mState) {
         if (mPayload)
            return Text::TemplateRt("{}({}, {})", mType, mState, Traits::Data {mPayload});
         else
            return Text::TemplateRt("{}({})", mType, mState);
      }
      else return Text::TemplateRt("{}()", mType);
   }

   /// Register all commonly used event types                                 
   void RegisterEvents() {
      (void) MetaDataOf<Events::WindowClose>();
      (void) MetaDataOf<Events::WindowFileDrop>();
      (void) MetaDataOf<Events::WindowMouseHoverIn>();
      (void) MetaDataOf<Events::WindowMouseHoverOut>();
      (void) MetaDataOf<Events::WindowResolutionChange>();
      (void) MetaDataOf<Events::WindowMinimize>();
      (void) MetaDataOf<Events::WindowMaximize>();
      (void) MetaDataOf<Events::WindowFocus>();
      (void) MetaDataOf<Events::WindowUnfocus>();
      (void) MetaDataOf<Events::WindowResize>();
      (void) MetaDataOf<Events::WindowMove>();
      (void) MetaDataOf<Events::WindowText>();

      (void) MetaDataOf<Events::MouseMoveHorizontal>();
      (void) MetaDataOf<Events::MouseMoveVertical>();
      (void) MetaDataOf<Events::MouseMove>();
      (void) MetaDataOf<Events::MouseScrollHorizontal>();
      (void) MetaDataOf<Events::MouseScrollVertical>();
      (void) MetaDataOf<Events::MouseScroll>();

      (void) MetaDataOf<Keys::Space>();
      (void) MetaDataOf<Keys::Apostrophe>();
      (void) MetaDataOf<Keys::Comma>();
      (void) MetaDataOf<Keys::Minus>();
      (void) MetaDataOf<Keys::Period>();
      (void) MetaDataOf<Keys::Slash>();
      (void) MetaDataOf<Keys::Hack>();
      (void) MetaDataOf<Keys::Semicolon>();
      (void) MetaDataOf<Keys::LeftBracket>();
      (void) MetaDataOf<Keys::RightBracket>();
      (void) MetaDataOf<Keys::Tilde>();

      (void) MetaDataOf<Keys::Main0>();
      (void) MetaDataOf<Keys::Main1>();
      (void) MetaDataOf<Keys::Main2>();
      (void) MetaDataOf<Keys::Main3>();
      (void) MetaDataOf<Keys::Main4>();
      (void) MetaDataOf<Keys::Main5>();
      (void) MetaDataOf<Keys::Main6>();
      (void) MetaDataOf<Keys::Main7>();
      (void) MetaDataOf<Keys::Main8>();
      (void) MetaDataOf<Keys::Main9>();

      (void) MetaDataOf<Keys::A>();
      (void) MetaDataOf<Keys::B>();
      (void) MetaDataOf<Keys::C>();
      (void) MetaDataOf<Keys::D>();
      (void) MetaDataOf<Keys::E>();
      (void) MetaDataOf<Keys::F>();
      (void) MetaDataOf<Keys::G>();
      (void) MetaDataOf<Keys::H>();
      (void) MetaDataOf<Keys::I>();
      (void) MetaDataOf<Keys::J>();
      (void) MetaDataOf<Keys::K>();
      (void) MetaDataOf<Keys::L>();
      (void) MetaDataOf<Keys::M>();
      (void) MetaDataOf<Keys::N>();
      (void) MetaDataOf<Keys::O>();
      (void) MetaDataOf<Keys::P>();
      (void) MetaDataOf<Keys::Q>();
      (void) MetaDataOf<Keys::R>();
      (void) MetaDataOf<Keys::S>();
      (void) MetaDataOf<Keys::T>();
      (void) MetaDataOf<Keys::U>();
      (void) MetaDataOf<Keys::V>();
      (void) MetaDataOf<Keys::W>();
      (void) MetaDataOf<Keys::X>();
      (void) MetaDataOf<Keys::Y>();
      (void) MetaDataOf<Keys::Z>();

      (void) MetaDataOf<Keys::Escape>();
      (void) MetaDataOf<Keys::Enter>();
      (void) MetaDataOf<Keys::Tab>();
      (void) MetaDataOf<Keys::Back>();
      (void) MetaDataOf<Keys::Insert>();
      (void) MetaDataOf<Keys::Delete>();
      (void) MetaDataOf<Keys::PageUp>();
      (void) MetaDataOf<Keys::PageDown>();
      (void) MetaDataOf<Keys::Home>();
      (void) MetaDataOf<Keys::End>();

      (void) MetaDataOf<Keys::Left>();
      (void) MetaDataOf<Keys::Right>();
      (void) MetaDataOf<Keys::Up>();
      (void) MetaDataOf<Keys::Down>();

      (void) MetaDataOf<Keys::CapsLock>();
      (void) MetaDataOf<Keys::ScrollLock>();
      (void) MetaDataOf<Keys::NumLock>();
      (void) MetaDataOf<Keys::Print>();
      (void) MetaDataOf<Keys::Pause>();
             
      (void) MetaDataOf<Keys::F1>();
      (void) MetaDataOf<Keys::F2>();
      (void) MetaDataOf<Keys::F3>();
      (void) MetaDataOf<Keys::F4>();
      (void) MetaDataOf<Keys::F5>();
      (void) MetaDataOf<Keys::F6>();
      (void) MetaDataOf<Keys::F7>();
      (void) MetaDataOf<Keys::F8>();
      (void) MetaDataOf<Keys::F9>();
      (void) MetaDataOf<Keys::F10>();
      (void) MetaDataOf<Keys::F11>();
      (void) MetaDataOf<Keys::F12>();
      (void) MetaDataOf<Keys::F13>();
      (void) MetaDataOf<Keys::F14>();
      (void) MetaDataOf<Keys::F15>();
      (void) MetaDataOf<Keys::F16>();
      (void) MetaDataOf<Keys::F17>();
      (void) MetaDataOf<Keys::F18>();
      (void) MetaDataOf<Keys::F19>();
      (void) MetaDataOf<Keys::F20>();
      (void) MetaDataOf<Keys::F21>();
      (void) MetaDataOf<Keys::F22>();
      (void) MetaDataOf<Keys::F23>();
      (void) MetaDataOf<Keys::F24>();

      (void) MetaDataOf<Keys::Numpad0>();
      (void) MetaDataOf<Keys::Numpad1>();
      (void) MetaDataOf<Keys::Numpad2>();
      (void) MetaDataOf<Keys::Numpad3>();
      (void) MetaDataOf<Keys::Numpad4>();
      (void) MetaDataOf<Keys::Numpad5>();
      (void) MetaDataOf<Keys::Numpad6>();
      (void) MetaDataOf<Keys::Numpad7>();
      (void) MetaDataOf<Keys::Numpad8>();
      (void) MetaDataOf<Keys::Numpad9>();

      (void) MetaDataOf<Keys::NumpadEqual>();
      (void) MetaDataOf<Keys::NumpadDecimal>();
      (void) MetaDataOf<Keys::NumpadDivide>();
      (void) MetaDataOf<Keys::NumpadMultiply>();
      (void) MetaDataOf<Keys::NumpadSubtract>();
      (void) MetaDataOf<Keys::NumpadAdd>();
      (void) MetaDataOf<Keys::NumpadEnter>();

      (void) MetaDataOf<Keys::LeftShift>();
      (void) MetaDataOf<Keys::RightShift>();

      (void) MetaDataOf<Keys::LeftControl>();
      (void) MetaDataOf<Keys::RightControl>();

      (void) MetaDataOf<Keys::LeftAlt>();
      (void) MetaDataOf<Keys::RightAlt>();

      (void) MetaDataOf<Keys::LeftMouse>();
      (void) MetaDataOf<Keys::MiddleMouse>();
      (void) MetaDataOf<Keys::RightMouse>();
      (void) MetaDataOf<Keys::Mouse4>();
      (void) MetaDataOf<Keys::Mouse5>();
      (void) MetaDataOf<Keys::Mouse6>();
      (void) MetaDataOf<Keys::Mouse7>();
      (void) MetaDataOf<Keys::Mouse8>();

      (void) MetaDataOf<Keys::Joystick1>();
      (void) MetaDataOf<Keys::Joystick2>();
      (void) MetaDataOf<Keys::Joystick3>();
      (void) MetaDataOf<Keys::Joystick4>();
      (void) MetaDataOf<Keys::Joystick5>();
      (void) MetaDataOf<Keys::Joystick6>();
      (void) MetaDataOf<Keys::Joystick7>();
      (void) MetaDataOf<Keys::Joystick8>();
      (void) MetaDataOf<Keys::Joystick9>();
      (void) MetaDataOf<Keys::Joystick10>();
      (void) MetaDataOf<Keys::Joystick11>();
      (void) MetaDataOf<Keys::Joystick12>();
      (void) MetaDataOf<Keys::Joystick13>();
      (void) MetaDataOf<Keys::Joystick14>();
      (void) MetaDataOf<Keys::Joystick15>();
      (void) MetaDataOf<Keys::Joystick16>();
   }

} // namespace Langulus