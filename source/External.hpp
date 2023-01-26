///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"

namespace Langulus::A
{

   ///                                                                        
   ///   Abstract platform module                                             
   ///                                                                        
   struct PlatformModule : Entity::Module {
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;
   };

   ///                                                                        
   ///   Abstract platform unit                                               
   ///                                                                        
   struct Platform : Entity::Unit {
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;
   };
   
   ///                                                                        
   ///   Abstract platform window unit                                        
   ///                                                                        
   struct Window : Platform {
      LANGULUS(PRODUCER) PlatformModule;
      LANGULUS_BASES(Platform);
      using Platform::Platform;
   };

   ///                                                                        
   ///   Abstract physical module                                             
   ///                                                                        
   struct PhysicalModule : Entity::Module {
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;
   };

   ///                                                                        
   ///   Abstract physical unit                                               
   ///                                                                        
   struct Physical : Entity::Unit {
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract physical world unit                                         
   ///                                                                        
   struct World : Physical {
      LANGULUS(PRODUCER) PhysicalModule;
      LANGULUS_BASES(Physical);
      using Physical::Physical;
   };
   
   ///                                                                        
   ///   Abstract physical instance unit                                      
   ///                                                                        
   struct Instance : Physical {
      LANGULUS(PRODUCER) World;
      LANGULUS_BASES(Physical);
      using Physical::Physical;
   };
   
   ///                                                                        
   ///   Abstract file system module                                          
   ///                                                                        
   struct FileSystem : Entity::Module {
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;
   };

   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   struct File : Entity::Unit {
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;

      NOD() virtual Anyness::Any ReadAs(Anyness::DMeta) const = 0;
      
      template<class T>
      NOD() T ReadAs() const {
         return ReadAs(RTTI::MetaData::Of<T>()).template As<T>();
      }
   };

   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   struct Folder : Entity::Unit {
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract graphics module                                             
   ///                                                                        
   struct GraphicsModule : Entity::Module {
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;
   };

   ///                                                                        
   ///   Abstract graphics units                                              
   ///                                                                        
   struct Graphics : Entity::Unit {
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;
   };

   ///                                                                        
   ///   Abstract graphics renderer                                           
   ///                                                                        
   struct Renderer : Graphics {
      LANGULUS(PRODUCER) GraphicsModule;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics layer                                              
   ///                                                                        
   struct Layer : Graphics {
      LANGULUS(PRODUCER) Renderer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics camera                                             
   ///                                                                        
   struct Camera : Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics renderable                                         
   ///                                                                        
   struct Renderable : Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract graphics light                                              
   ///                                                                        
   struct Light : Graphics {
      LANGULUS(PRODUCER) Layer;
      LANGULUS_BASES(Graphics);
      using Graphics::Graphics;
   };

   ///                                                                        
   ///   Abstract content module                                              
   ///                                                                        
   struct ContentModule : Entity::Module {
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;
   };

   ///                                                                        
   ///   Abstract content unit                                                
   ///                                                                        
   struct Content : Entity::Unit, Flow::ProducedFrom<ContentModule> {
      LANGULUS_BASES(Entity::Unit);

      Content(Anyness::DMeta type, 
              ContentModule* producer, 
              const Anyness::Any& descriptor)
         : Unit {type, descriptor}
         , ProducedFrom {producer, descriptor} {}

      using DataMap = Anyness::TUnorderedMap<Anyness::TMeta, Anyness::Any>;

      virtual const DataMap& GetData() const noexcept = 0;

      NOD() Anyness::Trait* GetData(Anyness::TMeta, Offset = 0);
      NOD() const Anyness::Trait* GetData(Anyness::TMeta, Offset = 0) const;

      template<CT::Trait T>
      NOD() Anyness::Trait* GetData(Offset = 0);
      template<CT::Trait T>
      NOD() const Anyness::Trait* GetData(Offset = 0) const;
   };

   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   struct Geometry : Content {
      LANGULUS(PRODUCER) ContentModule;
      LANGULUS_BASES(Content);
      using Content::Content;

      virtual Anyness::DMeta GetTopology() const noexcept = 0;
   };

   ///                                                                        
   ///   Abstract material content                                            
   ///                                                                        
   struct Material : Content {
      LANGULUS(PRODUCER) ContentModule;
      LANGULUS_BASES(Content);
      using Content::Content;
   };

   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   struct Texture : Content {
      LANGULUS(PRODUCER) ContentModule;
      LANGULUS_BASES(Content);
      using Content::Content;

      virtual Anyness::DMeta GetFormat() const noexcept = 0;
   };

} // namespace Langulus::A

namespace Langulus::CT
{

   /// A concept for any kind of a graphics unit                              
   template<class T>
   concept Graphics = DerivedFrom<T, A::Graphics>;

   /// A concept for any kind of a physical unit                              
   template<class T>
   concept Physical = DerivedFrom<T, A::Physical>;

   /// A concept for any kind of content unit                                 
   template<class T>
   concept Content = DerivedFrom<T, A::Content>;

   /// A concept for any kind of image content unit                           
   template<class T>
   concept Texture = DerivedFrom<T, A::Texture>;

   /// A concept for any kind of geometric content unit                       
   template<class T>
   concept Geometry = DerivedFrom<T, A::Geometry>;

} // namespace Langulus::CT