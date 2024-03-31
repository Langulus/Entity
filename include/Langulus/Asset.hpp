///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
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
      const Ref<A::Folder>& GetFolder() const noexcept;
   };


   ///                                                                        
   ///   Abstract asset unit                                                  
   ///                                                                        
   struct Asset : virtual Unit, virtual ProducedFrom<AssetModule> {
      LANGULUS(PRODUCER) AssetModule;
      LANGULUS_BASES(Unit);
      
      using Data = Any;
      using DataList = TAny<Data>;
      using DataListMap = TUnorderedMap<TMeta, DataList>;

   protected:
      // Map of lists of generated data                                 
      mutable DataListMap mDataListMap;

   public:
      Asset() : Resolvable {this} {}

      void Detach();

      virtual bool Generate(TMeta, Offset = 0) = 0;

      template<CT::TraitBased = Langulus::Trait, class B>
      requires CT::Block<Desem<B>>
      void Commit(B&&) const;

      template<CT::TraitBased = Langulus::Trait>
      NOD() const Data* GetData(Offset = 0) const noexcept;
      NOD() const Data* GetData(TMeta, Offset = 0) const noexcept;

      template<CT::TraitBased = Langulus::Trait>
      NOD() const DataList* GetDataList() const noexcept;
      NOD() const DataList* GetDataList(TMeta) const noexcept;

   protected:
      template<CT::TraitBased = Langulus::Trait>
      NOD() Data* GetData(Offset = 0) noexcept;
      NOD() Data* GetData(TMeta, Offset = 0) noexcept;

      template<CT::TraitBased = Langulus::Trait>
      NOD() DataList* GetDataList() noexcept;
      NOD() DataList* GetDataList(TMeta) noexcept;

      NOD() DataListMap const& GetDataListMap() const noexcept;
      NOD() DataListMap& GetDataListMap() noexcept;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of asset                                        
   template<class T>
   concept Asset = DerivedFrom<T, A::Asset>;

} // namespace Langulus::CT

#include "Asset.inl"
