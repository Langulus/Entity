///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Asset.hpp"


namespace Langulus::A
{
   
   /// Retrieve the asset module's repository folder                          
   ///   @return a reference to the folder interface                          
   LANGULUS(INLINED)
   const Ref<A::Folder>& AssetModule::GetFolder() const noexcept {
      return mFolder;
   }

   /// Get the entire content data map                                        
   ///   @attention this doesn't generate any data                            
   ///   @return a reference to the contents                                  
   LANGULUS(INLINED)
   Asset::DataListMap const& Asset::GetDataListMap() const noexcept {
      return mDataListMap;
   }

   LANGULUS(INLINED)
   Asset::DataListMap& Asset::GetDataListMap() noexcept {
      return mDataListMap;
   }

   /// Get a single data entry from the contents                              
   ///   @tparam T - the trait to search for                                  
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   template<CT::TraitBased T> LANGULUS(INLINED)
   Asset::Data* Asset::GetData(Offset index) noexcept {
      TMeta trait;
      if constexpr (CT::Trait<T>)
         trait = MetaTraitOf<T>();
      return GetData(trait, index);
   }

   template<CT::TraitBased T> LANGULUS(INLINED)
   Asset::Data const* Asset::GetData(Offset index) const noexcept {
      return const_cast<Asset*>(this)->GetData<T>(index);
   }

   /// Get a single data entry from the contents                              
   ///   @param trait - the trait to search for                               
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   LANGULUS(INLINED)
   Asset::Data* Asset::GetData(TMeta trait, Offset index) noexcept {
      if (not Generate(trait, index))
         return nullptr;

      const auto datalist = GetDataList(trait);
      return datalist and datalist->GetCount() > index
         ? &(*datalist)[index]
         : nullptr;
   }

   LANGULUS(INLINED)
   Asset::Data const* Asset::GetData(TMeta trait, Offset index) const noexcept {
      return const_cast<Asset*>(this)->GetData(trait, index);
   }

   /// Get a data list from the contents                                      
   ///   @tparam T - the trait to search for                                  
   ///   @return a pointer to the data list, or nullptr if none exists        
   template<CT::TraitBased T> LANGULUS(INLINED)
   Asset::DataList* Asset::GetDataList() noexcept {
      TMeta trait;
      if constexpr (CT::Trait<T>)
         trait = MetaTraitOf<T>();
      return GetDataList(trait);
   }

   template<CT::TraitBased T> LANGULUS(INLINED)
   Asset::DataList const* Asset::GetDataList() const noexcept {
      return const_cast<Asset*>(this)->GetDataList<T>();
   }

   /// Get a data list from the contents                                      
   ///   @param trait - the trait to search for                               
   ///   @return a pointer to the data list, or nullptr if none exists        
   LANGULUS(INLINED)
   Asset::DataList* Asset::GetDataList(TMeta trait) noexcept {
      if (not const_cast<Asset*>(this)->Generate(trait))
         return nullptr;

      const auto found = mDataListMap.FindIt(trait);
      return found ? found.mValue : nullptr;
   }

   LANGULUS(INLINED)
   const Asset::DataList* Asset::GetDataList(TMeta trait) const noexcept {
      return const_cast<Asset*>(this)->GetDataList(trait);
   }

   /// Commit data to an asset                                                
   ///   @attention data is always comitted, even if trait already exists     
   ///   @tparam T - trait (a.k.a. the intent we're commiting with)           
   ///      @attention default value leads all data to the no-trait bucket    
   ///   @tparam B - block of data to insert (semantically or not)            
   ///      @attention you can use references to external data, but beware    
   ///         that asset data is intrinsically paired with the asset         
   ///         descriptor, and any silent change in contents might cause      
   ///         disparity and confusion in asset libraries. Still useful when  
   ///         you don't use factories, and just want some local content      
   ///         representation.                                                
   ///   @param content - the data and semantic to commit                     
   template<CT::TraitBased T, class B> 
   requires CT::Block<Desem<B>> LANGULUS(INLINED)
   void Asset::Commit(B&& content) const {
      using S = SemanticOf<B>;
      TMeta trait;
      if constexpr (CT::Trait<T>)
         trait = MetaTraitOf<T>();

      const auto found = mDataListMap.FindIt(trait);
      if (found)
         *found.mValue << Any {S::Nest(content)};
      else
         mDataListMap.Insert(trait, S::Nest(content));
   }

} // namespace Langulus::A
