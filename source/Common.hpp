///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Flow/Flow.hpp>

#if defined(LANGULUS_EXPORT_ALL) || defined(LANGULUS_EXPORT_ENTITY)
   #define LANGULUS_API_ENTITY() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ENTITY() LANGULUS_IMPORT()
#endif

namespace Langulus::Entity
{

   using namespace ::Langulus::Anyness;

   using Flow::Verb;
   using Flow::Construct;
   using Flow::Charge;
   using Flow::Seek;
   using Flow::Resolvable;
   using Flow::Temporal;
   using Flow::Time;
   using Flow::Scope;
   using Flow::Code;

   //                                                                   
   // Some predeclarations:                                             

   struct Unit;
   class Thing;
   class Runtime;
   class Module;
   struct Hierarchy;

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

/// Make the rest of the code aware, that Langulus::Entity has been included  
#define LANGULUS_LIBRARY_ENTITY() 1
