///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Unit.hpp"

namespace Langulus::Entity
{

   /// Get the list of unit owners                                            
   ///   return the owners                                                    
   LANGULUS(ALWAYSINLINE)
   const Hierarchy& Unit::GetOwners() const noexcept {
      return mOwners;
   }

   /// Get a value from inside a trait by scanning owners and other units     
   ///   @param value - [out] value to set if trait was found                 
   ///   @return true if anything was written to value                        
   template<SeekStyle SEEK, CT::Trait T, CT::Data D>
   LANGULUS(ALWAYSINLINE)
   bool Unit::SeekValue(D& value, Offset offset) const {
      return SeekValue<SEEK, D>(MetaTrait::Of<T>(), value, offset);
   }

} // namespace Langulus::Entity