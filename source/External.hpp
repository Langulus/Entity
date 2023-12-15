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
      RTTI::DMeta mTopology {};
      // Double-sidedness                                               
      bool mBilateral {};
      // Texture mapping mode                                           
      Math::MapMode mTextureMapping {};

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
      RTTI::DMeta mFormat {};
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
   ///   Abstract platform window                                             
   ///                                                                        
   struct Window : Platform {
      LANGULUS(PRODUCER) PlatformModule;
      LANGULUS_BASES(Platform);
      using Platform::Platform;

      NOD() virtual void* GetNativeHandle() const noexcept = 0;
      NOD() virtual Math::Scale2 GetSize() const noexcept = 0;
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

      NOD() virtual bool Cull(const Math::LOD&) const noexcept = 0;
      NOD() virtual Math::Level GetLevel() const noexcept = 0;
      NOD() virtual Math::Mat4 GetModelTransform(const Math::LOD&) const noexcept = 0;
      NOD() virtual Math::Mat4 GetModelTransform(const Math::Level& = {}) const noexcept = 0;
      NOD() virtual Math::Mat4 GetViewTransform(const Math::LOD&) const noexcept = 0;
      NOD() virtual Math::Mat4 GetViewTransform(const Math::Level& = {}) const noexcept = 0;
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
         LANGULUS(PRODUCER) Module;
         LANGULUS_BASES(Entity::Unit);
         using Entity::Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI system                                                
      ///                                                                     
      struct System : Unit {
         LANGULUS_BASES(Unit);
         using Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI button                                                
      ///                                                                     
      struct Button : Unit {
         LANGULUS(PRODUCER) System;
         LANGULUS_BASES(Unit);
         using Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI text field                                            
      ///                                                                     
      struct Text : Unit {
         LANGULUS(PRODUCER) System;
         LANGULUS_BASES(Unit);
         using Unit::Unit;
      };

      ///                                                                     
      ///   Abstract UI input field                                           
      ///                                                                     
      struct Input : Unit {
         LANGULUS(PRODUCER) System;
         LANGULUS_BASES(Unit);
         using Unit::Unit;
      };

   } // namespace Langulus::A::UI

   struct File;
   struct Folder;

   ///                                                                        
   ///   Abstract file system module                                          
   ///                                                                        
   struct FileSystem : Entity::Module {
   protected:
      // Working directory path                                         
      // This is the only full path that is exposed to the system,      
      // unless LANGULUS(PARANOID) is enabled                           
      Anyness::Path mWorkingPath;
      // Main data directory, for both reading and writing              
      // Relative to mWorkingPath                                       
      Anyness::Path mMainDataPath;

   public:
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;

      NOD() virtual Anyness::Ref<File>   GetFile  (const Anyness::Path&) = 0;
      NOD() virtual Anyness::Ref<Folder> GetFolder(const Anyness::Path&) = 0;

      NOD() const Anyness::Path& GetWorkingPath() const noexcept;
      NOD() const Anyness::Path& GetDataPath() const noexcept;
   };

   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   struct File : Entity::Unit {
   protected:
      Anyness::Path mFilePath;
      bool mExists {};
      Anyness::DMeta mFormat {};
      Size mByteCount {};
      bool mIsReadOnly {};

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;
      //~File() override = default;

      NOD() bool Exists() const noexcept;
      NOD() bool IsReadOnly() const noexcept;
      NOD() const Anyness::Path& GetFilePath() const noexcept;
      NOD() Anyness::DMeta GetFormat() const noexcept;
      NOD() Size GetBytesize() const noexcept;

      NOD() virtual Anyness::Any ReadAs(Anyness::DMeta) const = 0;
      
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
         virtual ~Reader() = default;

         virtual Offset Read(Anyness::Block&) = 0;
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
         virtual ~Writer() = default;

         virtual Offset Write(const Anyness::Block&) = 0;
      };

      NOD() virtual Anyness::Ptr<Reader> NewReader() = 0;
      NOD() virtual Anyness::Ptr<Writer> NewWriter(bool append) = 0;
   };

   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   struct Folder : Entity::Unit {
   protected:
      Anyness::Path mFolderPath;
      bool mExists = false;
      bool mIsReadOnly = false;

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;
      //~Folder() override = default;

      NOD() bool Exists() const noexcept;
      NOD() bool IsReadOnly() const noexcept;
      NOD() const Anyness::Path& GetFolderPath() const noexcept;

      NOD() virtual Anyness::Ref<File>   GetFile  (const Anyness::Path&) const = 0;
      NOD() virtual Anyness::Ref<Folder> GetFolder(const Anyness::Path&) const = 0;
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
   ///   Abstract asset module                                                
   ///                                                                        
   struct AssetModule : Entity::Module {
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;
   };

   ///                                                                        
   ///   Abstract asset unit                                                  
   ///                                                                        
   struct Asset : Entity::Unit, Flow::ProducedFrom<AssetModule> {
      LANGULUS(PRODUCER) AssetModule;
      LANGULUS_BASES(Entity::Unit);
      
      Asset(RTTI::DMeta, AssetModule*, const Anyness::Neat&);

      using Data = Anyness::Any;
      using DataList = Anyness::TAny<Data>;
      using DataListMap = Anyness::TUnorderedMap<Anyness::TMeta, DataList>;

   protected:
      NOD() const DataListMap& GetDataListMap() const noexcept;

      // Map of lists of generated data                                 
      DataListMap mDataListMap;

   public:
      virtual bool Generate(Anyness::TMeta, Offset = 0) = 0;

      template<CT::Trait T, CT::Semantic S>
      void Commit(S&&);

      template<CT::Trait T>
      NOD() const Data* GetData(Offset = 0) const noexcept;
      NOD() const Data* GetData(Anyness::TMeta, Offset = 0) const noexcept;

      template<CT::Trait T>
      NOD() const DataList* GetDataList() const noexcept;
      NOD() const DataList* GetDataList(Anyness::TMeta) const noexcept;
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
      template<CT::Topology T>
      NOD() bool CheckTopology() const;
      NOD() Anyness::DMeta GetTopology() const noexcept;

      NOD() Math::MapMode GetTextureMapper() const noexcept;

      NOD() const MeshView& GetView() const noexcept;
      NOD() MeshView& GetView() noexcept;

      NOD() virtual Anyness::Ref<Mesh> GetLOD(const Math::LOD&) const = 0;

      NOD() Math::Vec2u InnerGetIndices(const Data*, const Math::Vec2u&) const;
      NOD() Math::Vec3u InnerGetIndices(const Data*, const Math::Vec3u&) const;

      //                                                                
      NOD() bool MadeOfPoints() const noexcept;
      NOD() Count GetPointCount() const;
      template<CT::Trait T>
      NOD() Anyness::Any GetPointTrait(Offset) const;

      //                                                                
      NOD() bool MadeOfLines() const noexcept;
      NOD() Count GetLineCount() const;
      NOD() Math::Vec2u GetLineIndices(Offset) const;
      template<CT::Trait T>
      NOD() Anyness::Any GetLineTrait(Offset) const;

      //                                                                
      NOD() bool MadeOfTriangles() const noexcept;
      NOD() Count GetTriangleCount() const;
      NOD() Math::Vec3u GetTriangleIndices(Offset) const;
      template<CT::Trait T>
      NOD() Anyness::Any GetTriangleTrait(Offset) const;
   };

   ///                                                                        
   ///   Abstract material content                                            
   ///                                                                        
   struct Material : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

      NOD() virtual Anyness::Ref<Material> GetLOD(const Math::LOD&) const = 0;

      NOD() const Entity::TraitList& GetInputs(Flow::Rate) const;
      NOD() const Entity::TraitList& GetInputs(Offset) const;

      NOD() const Entity::TraitList& GetOutputs(Flow::Rate) const;
      NOD() const Entity::TraitList& GetOutputs(Offset) const;
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
      NOD() virtual Anyness::Ref<Image> GetLOD(const Math::LOD&) const = 0;
      NOD() virtual void* GetGPUHandle() const noexcept = 0;

      NOD() Anyness::DMeta GetFormat() const noexcept;
      NOD() const ImageView& GetView() const noexcept;
      NOD() ImageView& GetView() noexcept;

      template<class F>
      auto ForEachPixel(F&&) const;

      void Upload(CT::Semantic auto&&);
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
