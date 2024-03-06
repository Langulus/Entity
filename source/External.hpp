///                                                                           
/// Langulus::Entity                                                          
/// Copyright (c) 2013 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Module.hpp"
#include "Unit.hpp"
#include "Runtime.hpp"
#include <Anyness/Path.hpp>
#include <Flow/Factory.hpp>
#include <Flow/Rate.hpp>
#include <Math/LOD.hpp>
#include <Math/Mapping.hpp>
#include <Math/Scale.hpp>

LANGULUS_DEFINE_TRAIT(Cursor,
   "Enables or disables cursor for window");
LANGULUS_DEFINE_TRAIT(Monitor,
   "Monitor to which window is bound, making it fullscreen");
LANGULUS_DEFINE_TRAIT(MousePosition,
   "Mouse position, relative to a window");
LANGULUS_DEFINE_TRAIT(MouseScroll,
   "Mouse scroll for window");
LANGULUS_DEFINE_TRAIT(NativeWindowHandle,
   "Native window handle");
LANGULUS_DEFINE_TRAIT(Shader,
   "Shader unit");
LANGULUS_DEFINE_TRAIT(Material,
   "Material unit");
LANGULUS_DEFINE_TRAIT(Image,
   "Image unit");
LANGULUS_DEFINE_TRAIT(Mesh,
   "Mesh unit");
LANGULUS_DEFINE_TRAIT(FOV,
   "Horizontal field of view angle, usually a real number");
LANGULUS_DEFINE_TRAIT(AspectRatio,
   "Aspect ratio trait (width / height), usually a real number");
LANGULUS_DEFINE_TRAIT(Viewport,
   "Viewport and depth clipping, usually a Range4");

LANGULUS_EXCEPTION(Image);
LANGULUS_EXCEPTION(Mesh);


namespace Langulus
{
   using namespace Langulus::Flow;
   using namespace Langulus::Anyness;
   using namespace Langulus::Entity;
   using namespace Langulus::Math;

   ///                                                                        
   ///   Vertex/index buffer view                                             
   ///                                                                        
   struct MeshView {
      // Number of primitives                                           
      uint32_t mPrimitiveCount {};
      // Starting primitive                                             
      uint32_t mPrimitiveStart {};
      // Number of indices                                              
      uint32_t mIndexCount {};
      // Starting index                                                 
      uint32_t mIndexStart {};
      // Data topology                                                  
      DMeta mTopology {};
      // Double-sidedness                                               
      bool mBilateral {};
      // Texture mapping mode                                           
      MapMode mTextureMapping {};

      bool operator == (const MeshView&) const noexcept;

      NOD() MeshView Decay() const;
      NOD() Hash GetHash() const noexcept;
   };


   ///                                                                        
   ///   Universal pixel buffer view                                          
   ///                                                                        
   struct ImageView {
      uint32_t mWidth {1};
      uint32_t mHeight {1};
      uint32_t mDepth {1};
      uint32_t mFrames {1};
      DMeta mFormat {};
      // Reverse RGBA to BGRA                                           
      // This is not a scalable solution and would eventually fail      
      bool mReverseFormat {};

      bool operator == (const ImageView&) const noexcept;

      NOD() constexpr uint32_t GetPixelCount() const noexcept;
      NOD() constexpr uint32_t GetDimensionCount() const noexcept;
      NOD() Size GetPixelBytesize() const noexcept;
      NOD() Size GetBytesize() const noexcept;
      NOD() uint32_t GetChannelCount() const noexcept;
      NOD() Hash GetHash() const noexcept;
   };
   

   ///                                                                        
   ///   Shader stages                                                        
   ///                                                                        
   namespace ShaderStage
   {
      enum Enum : uint32_t {
         Vertex = 0,          // Vertex shader stage                    
         Geometry,            // Geometry shader stage                  
         TessCtrl,            // Tesselation control stage              
         TessEval,            // Tesselation evaluation stage           
         Pixel,               // Pixel shader stage                     
         Compute,             // Compute shader stage                   

         Counter              // Shader stage counter (keep at end)     
      };

      /// Shader names                                                        
      constexpr Token Names[Enum::Counter] = {
         "vertex",
         "geometry",
         "tesselation control",
         "tesselation evaluation",
         "fragment",
         "compute"
      };
   }


   ///                                                                        
   ///   Shader layout tokens                                                 
   ///                                                                        
   namespace ShaderToken
   {
      constexpr Token Version       = "//#VERSION\n";
      constexpr Token Defines       = "//#DEFINES\n";
      constexpr Token Input         = "//#INPUT\n";
      constexpr Token Output        = "//#OUTPUT\n";
      constexpr Token Dependencies  = "//#DEPENDENCY\n";
      constexpr Token Colorize      = "//#COLORIZE\n";
      constexpr Token Transform     = "//#TRANSFORM\n";
      constexpr Token Position      = "//#POSITION\n";
      constexpr Token Uniform       = "//#UNIFORM\n";
      constexpr Token Texturize     = "//#TEXTURIZE\n";
      constexpr Token Functions     = "//#FUNCTIONS\n";
   }

} // namespace Langulus

namespace Langulus::A
{

   ///                                                                        
   ///   Abstract platform module                                             
   ///                                                                        
   struct PlatformModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract platform unit                                               
   ///                                                                        
   struct Platform : Unit {
      LANGULUS_BASES(Unit);
      using Unit::Unit;
   };
   
   ///                                                                        
   ///   Abstract platform window                                             
   ///                                                                        
   struct Window : Platform {
      LANGULUS(PRODUCER) PlatformModule;
      LANGULUS_BASES(Platform);
      using Platform::Platform;

      NOD() virtual void* GetNativeHandle() const noexcept = 0;
      NOD() virtual Scale2 GetSize() const noexcept = 0;
      NOD() virtual bool IsMinimized() const noexcept = 0;
   };
   
   ///                                                                        
   ///   Abstract platform cursor                                             
   ///                                                                        
   struct Cursor : Platform {
      LANGULUS(PRODUCER) PlatformModule;
      LANGULUS_BASES(Platform);
      using Platform::Platform;
   };

   ///                                                                        
   ///   Abstract physical module                                             
   ///                                                                        
   struct PhysicalModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract physical unit                                               
   ///                                                                        
   struct Physical : Unit {
      LANGULUS_BASES(Unit);
      using Unit::Unit;
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

      NOD() virtual bool Cull(const LOD&) const noexcept = 0;
      NOD() virtual Level GetLevel() const noexcept = 0;
      NOD() virtual Mat4 GetModelTransform(const LOD&) const noexcept = 0;
      NOD() virtual Mat4 GetModelTransform(const Level& = {}) const noexcept = 0;
      NOD() virtual Mat4 GetViewTransform(const LOD&) const noexcept = 0;
      NOD() virtual Mat4 GetViewTransform(const Level& = {}) const noexcept = 0;
   };
   
   namespace UI
   {

      ///                                                                     
      ///   Abstract UI module                                                
      ///                                                                     
      struct Module : Entity::Module {
         LANGULUS_BASES(Entity::Module);
         using Entity::Module::Module;
      };

      ///                                                                     
      ///   Abstract UI unit                                                  
      ///                                                                     
      struct Unit : Entity::Unit {
         LANGULUS(PRODUCER) UI::Module;
         LANGULUS_BASES(Entity::Unit);
         using Entity::Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI system                                                
      ///                                                                     
      struct System : UI::Unit {
         LANGULUS_BASES(UI::Unit);
         using UI::Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI button                                                
      ///                                                                     
      struct Button : UI::Unit {
         LANGULUS(PRODUCER) UI::System;
         LANGULUS_BASES(UI::Unit);
         using UI::Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI text field                                            
      ///                                                                     
      struct Text : UI::Unit {
         LANGULUS(PRODUCER) UI::System;
         LANGULUS_BASES(UI::Unit);
         using UI::Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI input field                                           
      ///                                                                     
      struct Input : UI::Unit {
         LANGULUS(PRODUCER) UI::System;
         LANGULUS_BASES(UI::Unit);
         using UI::Unit::Unit;
      };

   } // namespace Langulus::A::UI

   struct File;
   struct Folder;

   ///                                                                        
   ///   Abstract file system module                                          
   ///                                                                        
   struct FileSystem : Module {
   protected:
      // Working directory path                                         
      // This is the only full path that is exposed to the system,      
      // unless LANGULUS(PARANOID) is enabled                           
      Path mWorkingPath;
      // Main data directory, for both reading and writing              
      // Relative to mWorkingPath                                       
      Path mMainDataPath;

   public:
      LANGULUS_BASES(Module);
      using Module::Module;

      NOD() virtual Ref<File>   GetFile  (const Path&) = 0;
      NOD() virtual Ref<Folder> GetFolder(const Path&) = 0;

      NOD() const Path& GetWorkingPath() const noexcept;
      NOD() const Path& GetDataPath() const noexcept;
   };

   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   struct File : Unit {
   protected:
      Path mFilePath;
      bool mExists {};
      DMeta mFormat {};
      Size mByteCount {};
      bool mIsReadOnly {};

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Unit);
      using Unit::Unit;

      NOD() bool Exists() const noexcept;
      NOD() bool IsReadOnly() const noexcept;
      NOD() const Path& GetFilePath() const noexcept;
      NOD() DMeta GetFormat() const noexcept;
      NOD() Size GetBytesize() const noexcept;

      NOD() virtual Any ReadAs(DMeta) const = 0;

      NOD() virtual Ref<File>    RelativeFile  (const Path&) const = 0;
      NOD() virtual Ref<Folder>  RelativeFolder(const Path&) const = 0;
      
      template<class T>
      NOD() T ReadAs() const;

      /// Abstract file reader stream                                         
      struct Reader {
      protected:
         File* mFile;
         Offset mProgress {};

      public:
         Reader() = delete;
         Reader(File* f)
            : mFile {f} {}

         File* GetFile() const noexcept { return mFile; }

         virtual Offset Read(Any&) = 0;
      };

      /// Abstract file writer stream                                         
      struct Writer {
      protected:
         File* mFile;
         Offset mProgress {};
         bool mAppend = false;

      public:
         Writer() = delete;
         Writer(File* f, bool append)
            : mFile {f}, mAppend {append} {}

         File* GetFile() const noexcept { return mFile; }

         virtual Offset Write(const Any&) = 0;
      };

      NOD() virtual Ref<Reader> NewReader() const = 0;
      NOD() virtual Ref<Writer> NewWriter(bool append) const = 0;
   };

   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   struct Folder : Unit {
   protected:
      Path mFolderPath;
      bool mExists = false;
      bool mIsReadOnly = false;

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Unit);
      using Unit::Unit;

      NOD() bool Exists() const noexcept;
      NOD() bool IsReadOnly() const noexcept;
      NOD() const Path& GetFolderPath() const noexcept;

      NOD() virtual Ref<File>   RelativeFile  (const Path&) const = 0;
      NOD() virtual Ref<Folder> RelativeFolder(const Path&) const = 0;
   };

   ///                                                                        
   ///   Abstract graphics module                                             
   ///                                                                        
   struct GraphicsModule : Module {
      LANGULUS_BASES(Module);
      using Module::Module;
   };

   ///                                                                        
   ///   Abstract graphics units                                              
   ///                                                                        
   struct Graphics : Unit {
      LANGULUS_BASES(Unit);
      using Unit::Unit;
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
   ///   Abstract asset module                                                
   ///                                                                        
   struct AssetModule : Module {
      LANGULUS_BASES(Entity::Module);
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

   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   struct Mesh : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

   protected:
      MeshView mView;

   public:
      template<CT::Topology, CT::Topology...>
      NOD() bool CheckTopology() const;
      NOD() DMeta GetTopology() const noexcept;

      NOD() MapMode GetTextureMapper() const noexcept;

      NOD() const MeshView& GetView() const noexcept;
      NOD() MeshView& GetView() noexcept;

      NOD() virtual Ref<Mesh> GetLOD(const LOD&) const = 0;

      NOD() Vec2u InnerGetIndices(const Data*, const Vec2u&) const;
      NOD() Vec3u InnerGetIndices(const Data*, const Vec3u&) const;

      //                                                                
      NOD() bool MadeOfPoints() const noexcept;
      NOD() Count GetPointCount() const;
      template<CT::Trait T>
      NOD() Any GetPointTrait(Offset) const;

      //                                                                
      NOD() bool MadeOfLines() const noexcept;
      NOD() Count GetLineCount() const;
      NOD() Vec2u GetLineIndices(Offset) const;
      template<CT::Trait T>
      NOD() Any GetLineTrait(Offset) const;

      //                                                                
      NOD() bool MadeOfTriangles() const noexcept;
      NOD() Count GetTriangleCount() const;
      NOD() Vec3u GetTriangleIndices(Offset) const;
      template<CT::Trait T>
      NOD() Any GetTriangleTrait(Offset) const;
   };

   ///                                                                        
   ///   Abstract material content                                            
   ///                                                                        
   struct Material : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

      NOD() virtual Ref<Material> GetLOD(const LOD&) const = 0;

      NOD() const TraitList& GetInputs(Rate) const;
      NOD() const TraitList& GetInputs(Offset) const;

      NOD() const TraitList& GetOutputs(Rate) const;
      NOD() const TraitList& GetOutputs(Offset) const;
   };

   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   struct Image : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

   protected:
      ImageView mView;

   public:
      NOD() virtual Ref<Image> GetLOD(const LOD&) const = 0;
      NOD() virtual void* GetGPUHandle() const noexcept = 0;

      NOD() DMeta GetFormat() const noexcept;
      NOD() const ImageView& GetView() const noexcept;
      NOD() ImageView& GetView() noexcept;

      template<class F>
      auto ForEachPixel(F&&) const;

      template<template<class> class S, CT::Block B>
      requires CT::Semantic<S<B>>
      void Upload(S<B>&&);
   };
   
   ///                                                                        
   ///   Abstract font content                                                
   ///                                                                        
   struct Font : Image {
      LANGULUS_BASES(Image);
      using Image::Image;
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
   concept Asset = DerivedFrom<T, A::Asset>;

   /// A concept for any kind of image content unit                           
   template<class T>
   concept Image = DerivedFrom<T, A::Image>;

   /// A concept for any kind of geometric content unit                       
   template<class T>
   concept Mesh = DerivedFrom<T, A::Mesh>;

} // namespace Langulus::CT
