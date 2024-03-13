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
   struct AssetModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract asset unit                                                  
   ///                                                                        
   struct Asset : Unit, ProducedFrom<AssetModule> {
      LANGULUS(PRODUCER) AssetModule;
      LANGULUS_BASES(Unit);
      
      Asset(DMeta, AssetModule*, const Neat&);

      using Data = Any;
      using DataList = TAny<Data>;
      using DataListMap = TUnorderedMap<TMeta, DataList>;

   protected:
      NOD() const DataListMap& GetDataListMap() const noexcept;

      // Map of lists of generated data                                 
      DataListMap mDataListMap;

   public:
      virtual bool Generate(TMeta, Offset = 0) = 0;

      template<CT::Trait, template<class> class S, CT::Block B>
      requires CT::Semantic<S<B>>
      void Commit(S<B>&&);

      template<CT::Trait>
      NOD() const Data* GetData(Offset = 0) const noexcept;
      NOD() const Data* GetData(TMeta, Offset = 0) const noexcept;

      template<CT::Trait>
      NOD() const DataList* GetDataList() const noexcept;
      NOD() const DataList* GetDataList(TMeta) const noexcept;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of asset                                        
   template<class T>
   concept Asset = DerivedFrom<T, A::Asset>;

} // namespace Langulus::CT

#include "Asset.inl"
