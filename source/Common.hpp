///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <LangulusFlow.hpp>
#include <LangulusMath.hpp>

#if defined(LANGULUS_EXPORT_ALL) || defined(LANGULUS_EXPORT_ENTITY)
   #define LANGULUS_API_ENTITY() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ENTITY() LANGULUS_IMPORT()
#endif

namespace Langulus::Entity
{

   using namespace ::Langulus::Anyness;

   using ::Langulus::Flow::Verb;
   using ::Langulus::Flow::Construct;
   using ::Langulus::Flow::Charge;
   using ::Langulus::Flow::Seek;
   using ::Langulus::Flow::Resolvable;
   using ::Langulus::Flow::Temporal;
   using ::Langulus::Flow::Time;
   using ::Langulus::Flow::Scope;
   using ::Langulus::Flow::Code;

   /// A text specialization, dedicated for natural language text             
   /// It is a placeholder type, that can be extended by external modules     
   struct Lingua : Text {
      LANGULUS(ABSTRACT) true;
   };

   //                                                                   
   // Some predeclarations:                                             

   struct Unit;
   class Thing;
   class Runtime;
   class Module;
   struct Hierarchy;

   using ModuleList = TAny<Module*>;
   using MetaList = TUnorderedSet<const RTTI::Meta*>;
   using UnitList = TAny<Ref<Unit>>;
   using UnitMap = TUnorderedMap<DMeta, UnitList>;
   using TraitList = TAny<Trait>;
   using TraitMap = TUnorderedMap<TMeta, TraitList>;

} // namespace Langulus::Entity

namespace Langulus::A
{

   struct PlatformModule;
   struct Platform;
   struct Window;
   struct Cursor;

   struct PhysicalModule;
   struct Physical;
   struct World;
   struct Instance;

   struct FileSystem;
   struct File;
   struct Folder;

   struct GraphicsModule;
   struct Graphics;
   struct Renderer;
   struct Layer;
   struct Camera;
   struct Renderable;
   struct Light;

   struct ContentModule;
   struct Content;
   struct Geometry;
   struct Material;
   struct Texture;

} // namespace Langulus::A