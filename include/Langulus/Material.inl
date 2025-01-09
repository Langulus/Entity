///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Material.hpp"


namespace Langulus::A
{
   
   /// Get input traits for a given rate (const)                              
   ///   @param rate - the rate                                               
   ///   @return the input trait list                                         
   LANGULUS(INLINED)
   auto Material::GetInputs(RefreshRate rate) const -> const TraitList& {
      return GetInputs(rate.GetInputIndex());
   }

   /// Get input traits for a given input offset (const)                      
   ///   @attention offset must be in the range [0; Rate::InputCount)         
   ///   @param rate - the input offset                                       
   ///   @return the input trait list                                         
   LANGULUS(INLINED)
   auto Material::GetInputs(Offset rate) const -> const TraitList& {
      LANGULUS_ASSUME(DevAssumes, rate < RefreshRate::InputCount,
         "Input offset out of range");
      return mInputs[rate];
   }

   /// Get output traits for a given rate (const)                             
   ///   @param rate - the rate                                               
   ///   @return the output trait list                                        
   LANGULUS(INLINED)
   auto Material::GetOutputs(RefreshRate rate) const -> const TraitList& {
      return GetOutputs(rate.GetInputIndex());
   }

   /// Get output traits for a given input offset (const)                     
   ///   @attention offset must be in the range [0; Rate::InputCount)         
   ///   @param rate - the input offset                                       
   ///   @return the output trait list                                        
   LANGULUS(INLINED)
   auto Material::GetOutputs(Offset rate) const -> const TraitList& {
      LANGULUS_ASSUME(DevAssumes, rate < RefreshRate::InputCount,
         "Input offset out of range");
      return mOutputs[rate];
   }

} // namespace Langulus::A
