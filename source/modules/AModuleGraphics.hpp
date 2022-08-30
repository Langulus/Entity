#pragma once
#include "AModuleContent.hpp"
#include "AModulePlatform.hpp"

LANGULUS_DECLARE_TRAIT(Color, "A general color trait");
LANGULUS_DECLARE_TRAIT(Material, "Material trait");
LANGULUS_DECLARE_TRAIT(Texture, "Texture trait");
LANGULUS_DECLARE_TRAIT(Model, "Geometry model trait");

LANGULUS_DECLARE_TRAIT_FILTERED(Perspective, 
	"Perspective state", DataID::Of<bool>);
LANGULUS_DECLARE_TRAIT_FILTERED(FOV, 
	"Horizontal field of view trait (in radians)", DataID::Of<real>);
LANGULUS_DECLARE_TRAIT_FILTERED(AspectRatio, 
	"Aspect ratio trait (width / height)", DataID::Of<real>);
LANGULUS_DECLARE_TRAIT_FILTERED(Viewport, 
	"Viewport and depth clipping range trait", DataID::Of<range4>);

LANGULUS_DECLARE_TRAIT_FILTERED(ProjectTransform, 
	"Camera projection transformation", AMatrix::ID);
LANGULUS_DECLARE_TRAIT_FILTERED(ViewTransform, 
	"Camera view transformation", AMatrix::ID);
LANGULUS_DECLARE_TRAIT_FILTERED(ViewProjectTransform, 
	"Camera View * Projection transformation", AMatrix::ID);
LANGULUS_DECLARE_TRAIT_FILTERED(ModelViewProjectTransform, 
	"Model * Camera View * Camera Projection transformation", AMatrix::ID);

LANGULUS_DECLARE_TRAIT_FILTERED(ProjectTransformInverted, 
	"Camera projection transformation (inverted)", AMatrix::ID);
LANGULUS_DECLARE_TRAIT_FILTERED(ViewTransformInverted, 
	"Camera view transformation (inverted)", AMatrix::ID);
LANGULUS_DECLARE_TRAIT_FILTERED(ViewProjectTransformInverted, 
	"Camera View * Projection transformation (inverted)", AMatrix::ID);
LANGULUS_DECLARE_TRAIT_FILTERED(ModelViewProjectTransformInverted, 
	"Model * Camera View * Camera Projection transformation (inverted)", AMatrix::ID);

LANGULUS_DECLARE_EXCEPTION(Graphics);

namespace PCFW
{

	class AContent;
	class AWindow;

	///																								
	///	ABSTRACT GRAPHICS MODULE 															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleGraphics : public AModule {
		REFLECT(AModuleGraphics);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT GRAPHICS UNIT 																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AUnitGraphics : public AUnit {
		REFLECT(AUnitGraphics);
		using AUnit::AUnit;
	};


	///																								
	///	ABSTRACT RENDERER																		
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ARenderer : public AUnitGraphics {
	protected:
		Ptr<const AWindow> mWindow;

	public:
		REFLECT(ARenderer);
		using AUnitGraphics::AUnitGraphics;

		virtual void Initialize(AWindow*) = 0;
		virtual void Resize(const vec2&) = 0;
		virtual void Update() = 0;

		PC_SET(Window);
	};


	///																								
	///	ABSTRACT VISUAL LAYER																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AVisualLayer : public AUnitGraphics {
		REFLECT(AVisualLayer);
		using AUnitGraphics::AUnitGraphics;

		/// The layer style determines how the scene will be compiled				
		/// Combine these flags to configure the layer to your needs				
		enum Style {
			/// Batched layers are compiled for optimal performance, by grouping	
			/// all similar renderables and drawing them at once						
			/// This is the opposite of hierarchical rendering, because	it			
			/// destroys the order in which renderables appear							
			/// It is best suited for non-blended depth-tested scenes				
			Batched = 0,

			/// Hierarchical layers preserve the order in which elements occur	
			/// It is the opposite of batched rendering, because structure isn't	
			/// lost. This style is a bit less efficient, but is mandatory for	
			/// rendering UI for example														
			Hierarchical = 1,

			/// A multilevel layer supports instances that are not in the default
			/// human level. It is useful for rendering objects of the size of	
			/// the universe, or of the size of atoms, depending on the camera	
			/// configuration. Works by rendering the biggest levels first,		
			/// working down to the camera's capabilities, clearing the depth		
			/// after each successive level. This way one can seamlessly			
			/// compose infinitely complex scenes. Needless to say, this incurs	
			/// some performance penalty														
			Multilevel = 2,

			/// If enabled, will separate light computation on a different pass	
			/// Significantly improves performance on scenes with complex			
			/// lighting and shadowing															
			DeferredLights = 4,

			/// If enabled will sort instances by distance to camera, before		
			/// committing them for rendering												
			Sorted = 8,

			/// The default visual layer style												
			Default = Batched | Multilevel
		};

	protected:
		Style mStyle = Style::Default;

	public:
		PC_SET(Style);
	};


	///																								
	///	ABSTRACT RENDERABLE																	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ARenderable : public AUnitGraphics {
		REFLECT(ARenderable);
		using AUnitGraphics::AUnitGraphics;
	};


	///																								
	///	ABSTRACT LIGHT																			
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ALight : public AUnitGraphics {
		REFLECT(ALight);
		using AUnitGraphics::AUnitGraphics;
	};


	///																								
	///	ABSTRACT CAMERA																		
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ACamera : public AUnitGraphics {
	protected:
		// Whether or not a perspective projection is used						
		bool mPerspective = true;
		// The projection matrix														
		mat4 mProjection;
		// Clipping range in all directions, including depth					
		range4 mViewport = {{0, 0, 0.1, 0}, {720, 480, 1000, 0}};
		// Horizontal field of view, in radians									
		real mFOV = pcD2R(90.0f);
		// Aspect ratio (width / height)												
		real mAspectRatio = 720.0f / 480.0f;
		// Human retina is 32 milimeters (10^-3) across, which means that	
		// we can observe stuff slightly smaller than human octave			
		LevelRange mObservableRange {Level::Default, Level::Max};
		// Eye separation. Stereo if more/less than zero						
		real mEyeSeparation = 0;

	public:
		REFLECT(ACamera);
		using AUnitGraphics::AUnitGraphics;

		PC_SET(Projection);
		PC_SET(Viewport);
		PC_SET(FOV);
		PC_SET(AspectRatio);
		PC_SET(ObservableRange);
		PC_SET(EyeSeparation);
		PC_SET(Perspective);
	};


	///																								
	///	VRAM CONTENT MIRROR																	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) IContentVRAM : public AUnitGraphics {
	protected:
		Ref<const AContent> mOriginalContent;
		bool mContentMirrored = false;

	public:
		REFLECT(IContentVRAM);
		using AUnitGraphics::AUnitGraphics;

		PC_GET(OriginalContent);

		/// VRAM content hash is the same as the original content					
		NOD() inline Hash GetHash() const {
			return mOriginalContent->GetHash();
		}

		/// Compare VRAM by comparing original contents									
		inline bool operator == (const ME& other) const noexcept {
			return mOriginalContent == other.mOriginalContent;
		}

		/// Compare VRAM by comparing original contents									
		inline bool operator != (const ME& other) const noexcept {
			return !(*this == other);
		}
	};

	/// Helper function for converting colors to floats								
	///	@param color - the color to convert to GLSL color							
	///	@return GLSL color type																
	inline vec4 pcColorToRealVector(const Any& color) {
		switch (color.GetDataSwitch()) {
		case DataID::Switch<red8>():
			return rgba(color.As<red8>(), 0, 0, 255).Real();
		case DataID::Switch<green8>():
			return rgba(0, color.As<green8>(), 0, 255).Real();
		case DataID::Switch<blue8>():
			return rgba(0, 0, color.As<blue8>(), 255).Real();
		case DataID::Switch<alpha8>():
			return rgba(0, 0, 0, color.As<alpha8>()).Real();
		case DataID::Switch<rgba32>():
			return color.As<rgba>().Real();
		case DataID::Switch<rgb24>():
			return rgba(color.As<rgb>(), 255).Real();
		case DataID::Switch<rgb96>():
			return rgba128(color.As<rgb96>(), 1);
		case DataID::Switch<rgba128>():
			return color.As<rgba128>();
		case DataID::Switch<red32>():
			return rgba128(color.As<red32>(), 0, 0, 1);
		case DataID::Switch<green32>():
			return rgba128(0, color.As<green32>(), 0, 1);
		case DataID::Switch<blue32>():
			return rgba128(0, 0, color.As<blue32>(), 1);
		case DataID::Switch<alpha32>():
			return rgba128(0, 0, 0, color.As<alpha32>());
		default:
			throw Except::Graphics(pcLogFuncError
				<< "Unsupported color type: " << color.GetToken());
		}
	}

} // namespace PCFW