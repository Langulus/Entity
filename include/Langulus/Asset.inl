///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "Asset.hpp"


namespace Langulus::A
{
   
   /// Retrieve the asset module's repository folder                          
   ///   @return a reference to the folder interface                          
   LANGULUS(INLINED)
   auto AssetModule::GetFolder() const noexcept -> const Ref<A::Folder>& {
      return mFolder;
   }

   /// Get the entire content data map                                        
   ///   @attention this doesn't generate any data                            
   ///   @return a reference to the contents                                  
   LANGULUS(INLINED)
   auto Asset::GetDataListMap() const noexcept -> DataListMap const& {
      return mDataListMap;
   }

   LANGULUS(INLINED)
   auto Asset::GetDataListMap() noexcept -> DataListMap& {
      return mDataListMap;
   }

   /// Get a single data entry from the contents                              
   ///   @tparam T - the trait to search for                                  
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   template<CT::TraitBased T> LANGULUS(INLINED)
   auto Asset::GetData(Offset index) noexcept -> Data* {
      TMeta trait;
      if constexpr (CT::Trait<T>)
         trait = MetaTraitOf<T>();
      return GetData(trait, index);
   }

   template<CT::TraitBased T> LANGULUS(INLINED)
   auto Asset::GetData(Offset index) const noexcept -> Data const* {
      return const_cast<Asset*>(this)->GetData<T>(index);
   }

   /// Get a single data entry from the contents                              
   ///   @param trait - the trait to search for                               
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   LANGULUS(INLINED)
   auto Asset::GetData(TMeta trait, Offset index) noexcept -> Data* {
      if (not Generate(trait, index))
         return nullptr;

      const auto datalist = GetDataList(trait);
      return datalist and datalist->GetCount() > index
         ? &(*datalist)[index]
         : nullptr;
   }

   LANGULUS(INLINED)
   auto Asset::GetData(TMeta trait, Offset index) const noexcept -> Data const* {
      return const_cast<Asset*>(this)->GetData(trait, index);
   }

   /// Get a data list from the contents                                      
   ///   @tparam T - the trait to search for                                  
   ///   @return a pointer to the data list, or nullptr if none exists        
   template<CT::TraitBased T> LANGULUS(INLINED)
   auto Asset::GetDataList() noexcept -> DataList* {
      TMeta trait;
      if constexpr (CT::Trait<T>)
         trait = MetaTraitOf<T>();
      return GetDataList(trait);
   }

   template<CT::TraitBased T> LANGULUS(INLINED)
   auto Asset::GetDataList() const noexcept -> DataList const* {
      return const_cast<Asset*>(this)->GetDataList<T>();
   }

   /// Get a data list from the contents                                      
   ///   @param trait - the trait to search for                               
   ///   @return a pointer to the data list, or nullptr if none exists        
   LANGULUS(INLINED)
   auto Asset::GetDataList(TMeta trait) noexcept -> DataList* {
      if (not const_cast<Asset*>(this)->Generate(trait))
         return nullptr;

      const auto found = mDataListMap.FindIt(trait);
      return found ? &found.GetValue() : nullptr;
   }

   LANGULUS(INLINED)
   auto Asset::GetDataList(TMeta trait) const noexcept -> const DataList* {
      return const_cast<Asset*>(this)->GetDataList(trait);
   }

   /// Commit data to an asset                                                
   ///   @attention data is always comitted, even if trait already exists     
   ///   @tparam T - an optional trait type                                   
   ///      @attention default value leads all data to the no-trait bucket    
   ///   @param content - block of data to insert (with intents or not)       
   ///      @attention you can use references to external data, but beware    
   ///         that asset data is intrinsically paired with the asset         
   ///         descriptor, and any silent change in contents might cause      
   ///         disparity and confusion in asset libraries. Still useful when  
   ///         you don't use factories, and just want some local content      
   ///         representation.                                                
   template<CT::TraitBased T> LANGULUS(INLINED)
   void Asset::Commit(auto&& content) {
      TMeta trait;
      if constexpr (CT::Trait<T>)
         trait = MetaTraitOf<T>();

      using S = IntentOf<decltype(content)>;
      const auto found = mDataListMap.FindIt(trait);
      if (found)
         found.GetValue() << S::Nest(content);
      else
         mDataListMap.Insert(trait, S::Nest(content));
   }

   /// Reference or dereference the asset                                     
   ///   @param x - number of references to add/subtract                      
   ///   @return the number of remaining references                           
   LANGULUS(INLINED)
   Count Asset::Reference(int x) {
      const auto remaining = Unit::Reference(x);
      if (remaining == 1 and mProducer) {
         // If there's a producer, notify it when references reach 1    
         // - it means that the only reference is inside the factory,   
         // and it's the factory's responsibility to collect garbage,   
         // most likely on module Update().                             
         mProducer->RequestGarbageCollection();
         mDataListMap.Reset();
         ProducedFrom::Teardown();
      }

      return remaining;
   }

} // namespace Langulus::A
