///                                                                           
/// Langulus::Entity                                                          
/// Copyright(C) 2013 Dimo Markov <langulusteam@gmail.com>                    
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

namespace Langulus
{

   ///                                                                        
   ///   Vertex/index buffer view                                             
   ///                                                                        
   struct GeometryView {
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

      bool operator == (const GeometryView&) const noexcept;

      NOD() GeometryView Decay() const;
   };


   ///                                                                        
   ///   Universal pixel buffer view                                          
   ///                                                                        
   struct TextureView {
      uint32_t mWidth {1};
      uint32_t mHeight {1};
      uint32_t mDepth {1};
      uint32_t mFrames {1};
      RTTI::DMeta mFormat {};
      // Reverse RGBA to BGRA                                           
      // This is not a scalable solution and would eventually fail      
      bool mReverseFormat {};

      bool operator == (const TextureView&) const noexcept;

      NOD() constexpr uint32_t GetPixelCount() const noexcept;
      NOD() constexpr uint32_t GetDimensionCount() const noexcept;
      NOD() Size GetPixelBytesize() const noexcept;
      NOD() Size GetBytesize() const noexcept;
      NOD() uint32_t GetChannelCount() const noexcept;
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
      LANGULUS_BASES(Entity::Module);
      using Entity::Module::Module;

      NOD() virtual const File* GetFile(const Anyness::Path&) const = 0;
      NOD() virtual const Folder* GetFolder(const Anyness::Path&) const = 0;
   };

   ///                                                                        
   ///   Abstract file interface                                              
   ///                                                                        
   struct File : Entity::Unit {
   protected:
      Anyness::Path mFilePath;

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;

      NOD() const Anyness::Path& GetFilePath() const noexcept;

      NOD() virtual Anyness::Any ReadAs(Anyness::DMeta) const = 0;
      
      template<class T>
      NOD() T ReadAs() const;

      struct StreamIn {
         virtual Offset Read(Anyness::Block&) = 0;
      };

      struct StreamOut {
         virtual Offset Write(const Anyness::Block&) = 0;
      };

      NOD() virtual Anyness::Ptr<StreamIn>  NewStreamIn() = 0;
      NOD() virtual Anyness::Ptr<StreamOut> NewStreamOut() = 0;
   };

   ///                                                                        
   ///   Abstract folder interface                                            
   ///                                                                        
   struct Folder : Entity::Unit {
   protected:
      Anyness::Path mFolderPath;

   public:
      LANGULUS(PRODUCER) FileSystem;
      LANGULUS_BASES(Entity::Unit);
      using Entity::Unit::Unit;

      NOD() const Anyness::Path& GetFolderPath() const noexcept;

      NOD() virtual const File* GetFile(const Anyness::Path&) const = 0;
      NOD() virtual const Folder* GetFolder(const Anyness::Path&) const = 0;
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
      
      Asset(RTTI::DMeta, AssetModule*, const Anyness::Descriptor&);

      using Data = Anyness::Any;
      using DataList = Anyness::TAny<Data>;
      using DataListMap = Anyness::TUnorderedMap<Anyness::TMeta, DataList>;

   protected:
      DataListMap mDataListMap;

   public:
      NOD() const DataListMap& GetDataListMap() const noexcept;

      template<CT::Trait T>
      NOD() const Data* GetData(Offset = 0) const noexcept;
      NOD() const Data* GetData(Anyness::TMeta, Offset = 0) const noexcept;

      template<CT::Trait T>
      NOD() const DataList* GetDataList() const noexcept;
      NOD() const DataList* GetDataList(Anyness::TMeta) const noexcept;

      template<CT::Trait T, CT::Semantic S>
      void Commit(S&&);
   };

   ///                                                                        
   ///   Abstract geometry content                                            
   ///                                                                        
   struct Geometry : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

   protected:
      GeometryView mView;

   public:
      template<CT::Topology T>
      NOD() bool CheckTopology() const;
      NOD() Anyness::DMeta GetTopology() const noexcept;
      NOD() bool MadeOfTriangles() const noexcept;
      NOD() bool MadeOfLines() const noexcept;
      NOD() bool MadeOfPoints() const noexcept;

      NOD() Math::MapMode GetTextureMapper() const noexcept;

      NOD() const GeometryView& GetView() const noexcept;
      NOD() GeometryView& GetView() noexcept;

      NOD() virtual const Geometry* GetLOD(const Math::LOD&) const = 0;
   };

   ///                                                                        
   ///   Abstract material content                                            
   ///                                                                        
   struct Material : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

      NOD() virtual const Material* GetLOD(const Math::LOD&) const = 0;

      NOD() const Entity::TraitList& GetInputs(Flow::Rate) const noexcept;
   };

   ///                                                                        
   ///   Abstract texture content                                             
   ///                                                                        
   struct Texture : Asset {
      LANGULUS_BASES(Asset);
      using Asset::Asset;

   protected:
      TextureView mView;

   public:
      NOD() Anyness::DMeta GetFormat() const noexcept;
      NOD() const TextureView& GetView() const noexcept;
      NOD() TextureView& GetView() noexcept;

      NOD() virtual const Texture* GetLOD(const Math::LOD&) const = 0;
      NOD() virtual void* GetGPUHandle() const noexcept = 0;
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
   concept Texture = DerivedFrom<T, A::Texture>;

   /// A concept for any kind of geometric content unit                       
   template<class T>
   concept Geometry = DerivedFrom<T, A::Geometry>;

} // namespace Langulus::CT


LANGULUS_DEFINE_TRAIT(Shader,
   "Shader unit");
LANGULUS_DEFINE_TRAIT(Tesselation,
   "Tesselation level, usually an integer");
LANGULUS_DEFINE_TRAIT(Interpolator,
   "Interpolation mode");
LANGULUS_DEFINE_TRAIT(Material,
   "Material unit");
LANGULUS_DEFINE_TRAIT(Texture,
   "Texture unit");
LANGULUS_DEFINE_TRAIT(Geometry,
   "Geometry unit");
LANGULUS_DEFINE_TRAIT(FOV,
   "Horizontal field of view angle, usually a real number");
LANGULUS_DEFINE_TRAIT(AspectRatio,
   "Aspect ratio trait (width / height), usually a real number");
LANGULUS_DEFINE_TRAIT(Viewport,
   "Viewport and depth clipping, usually a Range4");