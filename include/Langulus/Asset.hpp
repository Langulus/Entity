///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#pragma once
#include "../Entity/Thing.hpp"
#include <Flow/Factory.hpp>
#include <Math/LOD.hpp>


namespace Langulus::A
{

   ///                                                                        
   ///   Abstract asset module                                                
   ///                                                                        
   struct AssetModule : virtual Module {
      LANGULUS_BASES(Module);
      AssetModule() : Resolvable {this}, Module {nullptr} {}

   protected:
      // Data folder, where assets will be saved or loaded from         
      Ref<A::Folder> mFolder;

   public:
      // @attention never delete stuff from this call! Do it on         
      // Update() instead                                               
      virtual void RequestGarbageCollection() = 0;
      auto GetFolder() const noexcept -> const Ref<A::Folder>&;
   };


   ///                                                                        
   ///   Abstract asset unit                                                  
   ///                                                                        
   struct Asset : virtual Unit, virtual ProducedFrom<AssetModule> {
      LANGULUS(PRODUCER) AssetModule;
      LANGULUS_BASES(Unit);
      
      using Data = Many;
      using DataList = TMany<Data>;
      using DataListMap = TUnorderedMap<TMeta, DataList>;

   protected:
      // Map of lists of generated data                                 
      DataListMap mDataListMap;

   public:
      Asset() : Resolvable {this} {}

      Count Reference(int);

      virtual bool Generate(TMeta, Offset = 0) { return true; }

      template<CT::TraitBased = Langulus::Trait>
      void Commit(auto&&);

      template<CT::TraitBased = Langulus::Trait>
      NOD() auto GetData(Offset = 0)       noexcept -> Data*;
      template<CT::TraitBased = Langulus::Trait>
      NOD() auto GetData(Offset = 0) const noexcept -> Data const*;

      NOD() auto GetData(TMeta, Offset = 0)       noexcept -> Data*;
      NOD() auto GetData(TMeta, Offset = 0) const noexcept -> Data const*;

      template<CT::TraitBased = Langulus::Trait>
      NOD() auto GetDataList()       noexcept -> DataList*;
      template<CT::TraitBased = Langulus::Trait>
      NOD() auto GetDataList() const noexcept -> DataList const*;

      NOD() auto GetDataList(TMeta)       noexcept -> DataList*;
      NOD() auto GetDataList(TMeta) const noexcept -> DataList const*;

      NOD() auto GetDataListMap()       noexcept -> DataListMap&;
      NOD() auto GetDataListMap() const noexcept -> DataListMap const&;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of asset                                        
   template<class T>
   concept Asset = DerivedFrom<T, A::Asset>;

} // namespace Langulus::CT

#include "Asset.inl"
