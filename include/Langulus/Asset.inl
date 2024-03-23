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

   /// Asset constructor                                                      
   ///   @param type - concrete type of the asset                             
   ///   @param producer - the asset library and producer                     
   ///   @param desc - messy descriptor for the content                       
   LANGULUS(INLINED)
   Asset::Asset(RTTI::DMeta type, AssetModule* producer, const Neat& desc)
      : Unit {type}
      , ProducedFrom<AssetModule> {producer, desc} {}

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
   template<CT::Trait T> LANGULUS(INLINED)
   Asset::Data const* Asset::GetData(Offset index) const noexcept {
      return GetData(MetaTraitOf<T>(), index);
   }

   template<CT::Trait T> LANGULUS(INLINED)
   Asset::Data* Asset::GetData(Offset index) noexcept {
      return GetData(MetaTraitOf<T>(), index);
   }

   /// Get a single data entry from the contents                              
   ///   @param trait - the trait to search for                               
   ///   @param index - the Nth data associated to the trait                  
   ///   @return a pointer to the data entry, or nullptr if none exists       
   LANGULUS(INLINED)
   Asset::Data const* Asset::GetData(TMeta trait, Offset index) const noexcept {
      return const_cast<Asset*>(this)->GetData(trait, index);
   }

   LANGULUS(INLINED)
   Asset::Data* Asset::GetData(TMeta trait, Offset index) noexcept {
      if (not Generate(trait, index))
         return nullptr;

      const auto datalist = GetDataList(trait);
      return datalist and datalist->GetCount() > index
         ? &(*datalist)[index]
         : nullptr;
   }

   /// Get a data list from the contents                                      
   ///   @tparam T - the trait to search for                                  
   ///   @return a pointer to the data list, or nullptr if none exists        
   template<CT::Trait T> LANGULUS(INLINED)
   Asset::DataList const* Asset::GetDataList() const noexcept {
      return GetDataList(MetaTraitOf<T>());
   }

   template<CT::Trait T> LANGULUS(INLINED)
   Asset::DataList* Asset::GetDataList() noexcept {
      return GetDataList(MetaTraitOf<T>());
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
   ///   @attention data is always commited, even if such trait already exists
   ///   @tparam T - data trait, the intent we're commiting with              
   ///   @param content - the data and semantic to commit                     
   template<CT::Trait T, template<class> class S, CT::Block B>
   requires CT::Semantic<S<B>> LANGULUS(INLINED)
   void Asset::Commit(S<B>&& content) const {
      const auto found = mDataListMap.FindIt(MetaTraitOf<T>());
      if (found)
         *found.mValue << Any {content.Forward()};
      else
         mDataListMap.Insert(MetaTraitOf<T>(), content.Forward());
   }

} // namespace Langulus::A
