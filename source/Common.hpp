///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <Anyness/TSet.hpp>
#include <Anyness/TMap.hpp>
#include <Anyness/TAny.hpp>
#include <Anyness/Ref.hpp>
#include <Anyness/Trait.hpp>
#include <Anyness/Path.hpp>
#include <Flow/Verb.hpp>
#include <Flow/Resolvable.hpp>

#if defined(LANGULUS_EXPORT_ALL) or defined(LANGULUS_EXPORT_ENTITY)
   #define LANGULUS_API_ENTITY() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ENTITY() LANGULUS_IMPORT()
#endif


namespace Langulus
{

   using namespace Anyness;
   using namespace Flow;

} // namespace Langulus::Entity

namespace Langulus::Entity
{

   class Thing;
   class Runtime;
   struct Hierarchy;

} // namespace Langulus::Entity

namespace Langulus::A
{

   struct Unit;
   class Module;

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

   namespace UI
   {

      struct Module;
      struct Unit;
      struct System;
      struct Button;
      struct Text;
      struct Input;

   } // namespace Langulus::A::UI

} // namespace Langulus::A

/// Make the rest of the code aware, that Langulus::Entity has been included  
#define LANGULUS_LIBRARY_ENTITY() 1
