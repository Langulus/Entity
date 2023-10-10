///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Flow/Verb.hpp>
#include <Flow/Resolvable.hpp>

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ENTITY)
   #define LANGULUS_API_ENTITY() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ENTITY() LANGULUS_IMPORT()
#endif

namespace Langulus::Entity
{

   using namespace ::Langulus::Anyness;

   using Flow::Verb;
   using Flow::Seek;
   using Flow::Resolvable;
   using Flow::Temporal;
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

   struct AssetModule;
   struct Asset;
   struct Mesh;
   struct Material;
   struct Image;

} // namespace Langulus::A

/// Make the rest of the code aware, that Langulus::Entity has been included  
#define LANGULUS_LIBRARY_ENTITY() 1
