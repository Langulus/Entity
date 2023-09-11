///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "IntegrateFramework.hpp"

namespace Langulus::Entity
{

   ///                                                                        
   ///   Context state                                                        
   ///                                                                        
   class ContextState {
      LANGULUS(POD) true;
      LANGULUS(NULLIFIABLE) true;
      LANGULUS(INFO) "Context state";
   public:
      enum Enum : uint8_t {
         // A default state                                             
         // It is basically a valid, mutable, enabled and local context 
         Default = 0,

         // Invalid contexts are recyclable, and marked for deletion    
         Invalid = 1,

         // An immutable context never changes                          
         Constant = 2,

         // A shared context will be synchronized in a network          
         Shared = 4,

         // A dynamic context is never optimized                        
         Dynamic = 8,

         // A disabled context doesn't update                           
         Disabled = 16,

         // A context is marked as busy                                 
         Busy = 32,

         // Will be automatically saved to a file                       
         Persistent = 64
      };

      LANGULUS_NAMED_VALUES(Enum) {
         {"Default", Default},
         {"Invalid", Invalid},
         {"Constant", Constant},
         {"Shared", Shared},
         {"Dynamic", Dynamic},
         {"Disabled", Disabled},
         {"Busy", Busy},
         {"Persistent", Persistent}
      };

      using Type = ::std::underlying_type_t<Enum>;

      Type mState {Default};

   public:
      constexpr ContextState() noexcept = default;
      constexpr ContextState(const Type&) noexcept;

      explicit constexpr operator bool() const noexcept;
      constexpr bool operator == (const ContextState&) const noexcept = default;

      NOD() constexpr ContextState operator + (const ContextState&) const noexcept;
      NOD() constexpr ContextState operator - (const ContextState&) const noexcept;
      constexpr ContextState& operator += (const ContextState&) noexcept;
      constexpr ContextState& operator -= (const ContextState&) noexcept;

      NOD() constexpr bool operator & (const ContextState&) const noexcept;
      NOD() constexpr bool operator % (const ContextState&) const noexcept;

      NOD() constexpr bool IsDefault() const noexcept;
      NOD() constexpr bool IsInvalid() const noexcept;
      NOD() constexpr bool IsConstant() const noexcept;
      NOD() constexpr bool IsShared() const noexcept;
      NOD() constexpr bool IsDynamic() const noexcept;
      NOD() constexpr bool IsDisabled() const noexcept;
      NOD() constexpr bool IsBusy() const noexcept;
      NOD() constexpr bool IsPersistent() const noexcept;

      constexpr void Reset() noexcept;
   };

} // namespace Langulus::Entity