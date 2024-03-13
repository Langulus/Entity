///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Material.hpp"


namespace Langulus::A
{
   
   /// Get input traits for a given rate (const)                              
   ///   @param rate - the rate                                               
   ///   @return the input trait list                                         
   LANGULUS(INLINED)
   const TraitList& Material::GetInputs(Rate rate) const {
      return GetInputs(rate.GetInputIndex());
   }

   /// Get input traits for a given input offset (const)                      
   ///   @attention offset must be in the range [0; Rate::InputCount)         
   ///   @param rate - the input offset                                       
   ///   @return the input trait list                                         
   LANGULUS(INLINED)
   const TraitList& Material::GetInputs(Offset rate) const {
      LANGULUS_ASSUME(DevAssumes,
         Asset::mDataListMap.ContainsKey(MetaOf<Traits::Input>()),
         "Material doesn't contain inputs");
      LANGULUS_ASSUME(DevAssumes,
         Asset::GetDataList<Traits::Input>()->GetCount() == Rate::InputCount,
         "Material doesn't contain the correct number of rates");
      LANGULUS_ASSUME(DevAssumes,
         rate < Rate::InputCount,
         "Input offset out of range");

      return *reinterpret_cast<const TraitList*>(
         Asset::GetData<Traits::Input>(rate));
   }

   /// Get output traits for a given rate (const)                             
   ///   @param rate - the rate                                               
   ///   @return the output trait list                                        
   LANGULUS(INLINED)
   const TraitList& Material::GetOutputs(Rate rate) const {
      return GetOutputs(rate.GetInputIndex());
   }

   /// Get output traits for a given input offset (const)                     
   ///   @attention offset must be in the range [0; Rate::InputCount)         
   ///   @param rate - the input offset                                       
   ///   @return the output trait list                                        
   LANGULUS(INLINED)
   const TraitList& Material::GetOutputs(Offset rate) const {
      LANGULUS_ASSUME(DevAssumes,
         Asset::mDataListMap.ContainsKey(MetaOf<Traits::Output>()),
         "Material doesn't contain inputs");
      LANGULUS_ASSUME(DevAssumes,
         Asset::GetDataList<Traits::Output>()->GetCount() == Rate::InputCount,
         "Material doesn't contain the correct number of rates");
      LANGULUS_ASSUME(DevAssumes,
         rate < Rate::InputCount,
         "Input offset out of range");

      return *reinterpret_cast<const TraitList*>(
         Asset::GetData<Traits::Output>(rate));
   }

} // namespace Langulus::A
