#pragma once
#include "../AModule.hpp"	
#include "../AUnit.hpp"

namespace PCFW
{

	///																								
	///	Blending modes																			
	///																								
	/// Represents different tactics for blending colors								
	///																								
	class LANGULUS_MODULE(FRAMEWORK) BlendMode : public TNamed<BlendMode, pcu8, false> {
		REFLECT(BlendMode);
	public:
		enum Enum : pcu8 {
			// No blending - final color becomes source color					
			None = 0,
			// No blending - final color becomes source color only			
			// if an alpha threshold is reached per pixel						
			Threshold = 1,
			// Final color = source color * alpha + destination color		
			// * (1 - alpha)																
			Normal = 2,
			// Final color = source color + destination color					
			Add = 3,
			// Final color = source color * destination color					
			Multiply = 4,

			Counter
		};

		/// Blending mode names																	
		static constexpr LiteralText Names[Enum::Counter] = {
			"None",
			"Threshold",
			"Normal",
			"Add",
			"Multiply"
		};

		static constexpr LiteralText GetToken(pcptr);

		using TNamed::TNamed;

		constexpr BlendMode(BlendMode::Enum initial)
			: TNamed {initial} {}
	};
	
	///																								
	///	Multipurpose mapper																	
	///																								
	/// Represents different tactics for mapping two properties to each other	
	///																								
	class LANGULUS_MODULE(FRAMEWORK) Mapper : public TNamed<Mapper, pcu8, false> {
		REFLECT(Mapper);
	public:
		enum Enum : pcu8 {
			// 3D texture coordinates are world coordinates, calculated		
			// at realtime in shaders													
			World = 0,
			// 3D texture coordinates are vertex coordinates, either			
			// precalculated from vertex positions, or calculated at			
			// realtime in shaders														
			Model,
			// 2D texture coordinates are camera-like projection,				
			// calculated at realtime in shaders									
			Projector,
			// 2D texture coordinates are screen coordinates, calculated	
			// at realtime in shaders													
			Screen,
			// 2D planar projection														
			Plane,
			// 2D cylindrical projection												
			Cylinder,
			// 2D spherical projection													
			Sphere,
			// 2D cubic projection (skybox)											
			Cubic,
			// 2D face mapping, where each face overlaps to the same			
			// texture at generation time												
			Face,
			// 2D injective projection, essentially packing each face 		
			// individually at generation time										
			Inject,
			// 2D contour projection, essentially mapping texture along 	
			// a spline at generation time											
			Contour,
			// 2D unfold projection, unwrapping texture along precut seams	
			// at generation time														
			Unfold,
			// The texture coordinates inside the generated geometry are	
			// used. This is usually done if one of the above projects		
			// the texture coordinates at generation time						
			Custom,

			Counter
		};

		/// Mapper names																			
		static constexpr LiteralText Names[Enum::Counter] = {
			"WorldMapper",
			"ModelMapper",
			"ProjectorMapper",
			"ScreenMapper",
			"PlaneMapper",
			"CylinderMapper",
			"SphereMapper",
			"CubicMapper",
			"FaceMapper",
			"InjectMapper",
			"ContourMapper",
			"UnfoldMapper",
			"CustomMapper"
		};

		static constexpr LiteralText GetToken(pcptr);

		using TNamed::TNamed;

		constexpr Mapper(Mapper::Enum initial)
			: TNamed {initial} {}
	};


	///																								
	/// Shader stages																				
	///																								
	namespace ShaderStage
	{
		enum Enum : pcptr {
			Vertex = 0,				// Vertex shader stage							
			Geometry,				// Geometry shader stage						
			TessCtrl,				// Tesselation control stage					
			TessEval,				// Tesselation evaluation stage				
			Pixel,					// Pixel shader stage							
			Compute,					// Compute shader stage							

			Counter					// Shader stage counter (keep at end)		
		};

		/// Shader names																			
		constexpr const char* Names[Enum::Counter] = {
			"vertex",
			"geometry",
			"tesselation control",
			"tesselation evaluation",
			"fragment",
			"compute"
		};
	}

	/// Shader tokens that correspond to shader templates								
	namespace ShaderToken {
		enum Enum {
			Version = 0,			// Version token									
			Defines,					// Defines token									
			Input,					// Input token										
			Output,					// Output token									
			Dependencies,			// Dependencies token							
			Colorize,				// Colors token									
			Transform,				// Transform token								
			Position,				// Vertex position								
			Uniform,					// Uniform variables								
			Texturize,				// For utilizing samplers						
			Functions,				// Function code token							

			Counter					// Shader token counter							
		};

		/// Template tokens for code generation											
		constexpr const char* Names[Enum::Counter] = {
			"//#VERSION\n",
			"//#DEFINES\n",
			"//#INPUT\n",
			"//#OUTPUT\n",
			"//#DEPENDENCY\n",
			"//#COLORIZE\n",
			"//#TRANSFORM\n",
			"//#POSITION\n",
			"//#UNIFORM\n",
			"//#TEXTURIZE\n",
			"//#FUNCTIONS\n"
		};
	}
	
	///																								
	///	REFRESH RATE																			
	///																								
	/// Represents the frequency at which stuff is precomputed at runtime		
	///																								
	class RefreshRate : public TNamed<RefreshRate, pcu8, false> {
		REFLECT(RefreshRate);

		enum Enum : pcu8 {
			PerAuto = 0,		// Automatically determined refresh rate		

			PerNone,				// A constant, essentially							

			PerTick,				// Updated once per time step						
			PerPass,				// Updated once per a render pass				
			PerCamera,			// Updated for each camera							
			PerLevel,			// Updated for each octave							
			PerRenderable,		// Updated for each renderable					
			PerInstance,		// Updated for each physical instance			

			// The following are mapped to ShaderStage::Enum					
			PerVertex,			// Updated in vertex shader						
			PerPrimitive,		// Updated in geometry shader						
			PerTessCtrl,		// Updated in tesselation control shader		
			PerTessEval,		// Updated in tesselation evaluate shader		
			PerPixel,			// Updated in pixel shader							

			Counter,				// Refresh rate counter								
			Error					// Used for error reporting						
		};

		static constexpr LiteralText Names[Enum::Counter] = {
			"PerAuto",

			"PerNone",

			"PerTick",
			"PerPass",
			"PerCamera",
			"PerLevel",
			"PerRenderable",
			"PerInstance",

			"PerVertex",
			"PerPrimitive",
			"PerTessCtrl",
			"PerTessEval",
			"PerPixel"
		};

		// Rates that are considered shader stages, mapped to ShaderStage	
		static constexpr pcptr StagesBegin = Enum::PerVertex;
		static constexpr pcptr StagesEnd = Enum::Counter;
		static constexpr pcptr StagesCount = StagesEnd - StagesBegin;

		// Rates that are considered uniforms										
		static constexpr pcptr UniformBegin = Enum::PerTick;
		static constexpr pcptr UniformEnd = StagesBegin;
		static constexpr pcptr UniformCount = UniformEnd - UniformBegin;

		// Rates that are considered inputs											
		static constexpr pcptr InputBegin = UniformBegin;
		static constexpr pcptr InputEnd = StagesEnd;
		static constexpr pcptr InputCount = InputEnd - InputBegin;

		// Rates that are considered static											
		static constexpr pcptr StaticUniformBegin = UniformBegin;
		static constexpr pcptr StaticUniformEnd = Enum::PerCamera;
		static constexpr pcptr StaticUniformCount = StaticUniformEnd - StaticUniformBegin;

		// Rates that are considered dynamic										
		static constexpr pcptr DynamicUniformBegin = StaticUniformEnd;
		static constexpr pcptr DynamicUniformEnd = UniformEnd;
		static constexpr pcptr DynamicUniformCount = DynamicUniformEnd - DynamicUniformBegin;

		using TNamed::TNamed;

		constexpr RefreshRate(RefreshRate::Enum initial)
			: TNamed {initial} {}

		// Avoid warnings with this constructor									
		constexpr RefreshRate(const int number)
			: TNamed {static_cast<pcu8>(number)} {}
		constexpr RefreshRate(const pcptr number)
			: TNamed {static_cast<pcu8>(number)} {}
		constexpr RefreshRate(const real number)
			: TNamed {static_cast<pcu8>(number)} {}

		explicit operator pcptr() const noexcept {
			return TNamed::mState;
		}

		static constexpr LiteralText GetToken(pcptr);
		constexpr bool IsUniform() const noexcept;
		constexpr bool IsStaticUniform() const noexcept;
		constexpr bool IsDynamicUniform() const noexcept;
		constexpr bool IsAttribute() const noexcept;
		constexpr bool IsInput() const noexcept;
		constexpr pcptr GetInputIndex() const;
		constexpr pcptr GetStaticUniformIndex() const;
		constexpr pcptr GetDynamicUniformIndex() const;
		constexpr bool IsShaderStage() const noexcept;
		constexpr ShaderStage::Enum GetStageIndex() const;
	};

	using RRate = RefreshRate;


	///																								
	///	Universal vertex/index buffer view												
	///																								
	struct LANGULUS_MODULE(FRAMEWORK) VertexView {
		// Number of primitives															
		pcptr mPCount = 0;
		// Starting primitive															
		pcptr mPStart = 0;
		// Number of indices																
		pcptr mICount = 0;
		// Starting index																	
		pcptr mIStart = 0;
		// Data topology																	
		DMeta mPrimitive = nullptr;
		// Double-sidedness																
		bool mBilateral = false;

		bool operator == (const VertexView&) const noexcept;
		VertexView Decay() const;
	};


	///																								
	///	Universal pixel buffer view														
	///																								
	struct LANGULUS_MODULE(FRAMEWORK) PixelView {
		pcptr mWidth = 1;
		pcptr mHeight = 1;
		pcptr mDepth = 1;
		pcptr mFrames = 1;
		DMeta mFormat = nullptr;
		// Reverse RGBA to BGRA															
		// This is not a scalable solution and would eventually fail		
		bool mReverseFormat = false;

		constexpr PixelView() noexcept = default;
		constexpr PixelView(
			pcptr w, pcptr h, pcptr d, pcptr f,
			DMeta format, bool reversed = false
		) noexcept;

		bool operator == (const PixelView&) const noexcept;

		NOD() constexpr pcptr CountPixels() const noexcept;
		NOD() constexpr pcptr CountDimensions() const noexcept;
		NOD() pcptr GetPixelSize() const noexcept;
		NOD() pcptr CountBytes() const noexcept;
		NOD() pcptr CountComponents() const noexcept;
	};


	///																								
	///	Universal sound buffer view														
	///																								
	struct LANGULUS_MODULE(FRAMEWORK) SoundView {};


	///																								
	///	Universal material view																
	///																								
	struct LANGULUS_MODULE(FRAMEWORK) MaterialView {};


	///																								
	///	ABSTRACT CONTENT MODULE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AModuleContent : public AModule {
		REFLECT(AModuleContent);
		using AModule::AModule;
	};


	///																								
	///	ABSTRACT CONTENT UNIT																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AContent : public AUnit {
	protected:
		Construct mConstruct;
		TAny<Trait> mData;
		TAny<Trait> mDataRange;
		bool mGenerated = false;
		bool mExplicitlyDefined = false;

	public:
		REFLECT(AContent);
		using AUnit::AUnit;

		NOD() Hash GetHash() const;
		bool operator == (const AContent&) const;
		bool operator != (const AContent&) const;

		virtual void Generate() = 0;
		inline void Generate() const {
			const_cast<AContent*>(this)->Generate();
		}
		virtual void Uninitialize();

		NOD() const Construct& GetConstruct() const noexcept;
		NOD() Construct& GetConstruct() noexcept;

		NOD() const TAny<Trait>& GetDataList() const noexcept;
		NOD() TAny<Trait>& GetDataList() noexcept;

		NOD() const TAny<Trait>& GetDataRangeList() const noexcept;
		NOD() TAny<Trait>& GetDataRangeList() noexcept;

		NOD() bool IsGenerated() const noexcept;
		NOD() bool IsExplicitlyDefined() const noexcept;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA = void>
		NOD() decltype(auto) GetDataAs(const Index& = uiFirst);
		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA = void>
		NOD() decltype(auto) GetDataAs(const Index& = uiFirst) const;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA = void>
		NOD() decltype(auto) GetDataRangeAs();
		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA = void>
		NOD() decltype(auto) GetDataRangeAs() const;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
		NOD() DATA GetTraitValue(const pcptr& = 0) const;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
		NOD() DATA GetTraitRangeValue() const;

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData DATA>
		void SetTraitValue(DATA&&, const pcptr& = 0);

		NOD() Trait* GetData(TMeta, pcptr = 0);
		NOD() const Trait* GetData(TMeta, pcptr = 0) const;

		NOD() Trait* GetDataRange(TMeta, pcptr = 0);
		NOD() const Trait* GetDataRange(TMeta, pcptr = 0) const;

		template<RTTI::ReflectedTrait TRAIT>
		NOD() Trait* GetData(pcptr = 0);
		template<RTTI::ReflectedTrait TRAIT>
		NOD() const Trait* GetData(pcptr = 0) const;

		template<RTTI::ReflectedTrait TRAIT>
		NOD() Trait* GetDataRange(pcptr = 0);
		template<RTTI::ReflectedTrait TRAIT>
		NOD() const Trait* GetDataRange(pcptr = 0) const;

		void SaveData(const TraitID, Any&&, Any&&);

		template<RTTI::ReflectedTrait TRAIT, RTTI::ReflectedData RANGE>
		void SaveData(Any&&, const RANGE&, const RANGE&);
		template<RTTI::ReflectedTrait TRAIT>
		void SaveData(Any&&);
	};


	using LodIndex = int;
	using AbsoluteLodIndex = pcptr;

	/// A helper structure that is used to fetch the correct LOD level			
	/// LOD level is simply a geometry, that is designed to represent a			
	/// zoomed-in or a zoomed-out region of another geometry							
	/// These regions can be generated on the fly, may reuse existing geometry	
	/// or may not exist at all																
	struct LANGULUS_MODULE(FRAMEWORK) LodState {
		Level mLevel;
		mat4 mView;
		mat4 mViewInverted;
		mat4 mModel;
		TFrustum<vec3> mFrustum;

		// Calculated after Transform()												
		mat4 mModelView;
		vec4 mOrigin;
		real mRadius;
		real mDistanceToSurface;
		real mLODIndex;

		static constexpr LodIndex MinIndex = -6;
		static constexpr LodIndex MaxIndex = 6;
		static constexpr LodIndex IndexCount = MaxIndex - MinIndex + 1;

		/// Recalculate LOD index for identity model matrix							
		void Transform() {
			mModel = mat4::Identity();
			mModelView = mViewInverted;
			mOrigin = {};
			mRadius = {};
			mDistanceToSurface = {};
			mLODIndex = 0;
		}

		/// Recalculate LOD index by specifying the model matrix						
		///	@param model - the model transformation									
		void Transform(const mat4& model) {
			mModel = model;
			mModelView = mModel * mViewInverted;
			mOrigin = mModelView.GetPosition();
			mRadius = mModelView.GetScale().HMax() * real(0.5);
			mDistanceToSurface = mOrigin.Length() - mRadius;
			mLODIndex = 0;
			if (mDistanceToSurface > 0 && mRadius > 0) {
				// View is outside the sphere											
				const real near = std::log10(mRadius / mDistanceToSurface);
				const real far = std::log10(mDistanceToSurface / mRadius);
				mLODIndex = pcClamp(near - far, real(MinIndex), real(MaxIndex));
			}
			else if (mDistanceToSurface < 0) {
				// Being inside the sphere always gives the most detail		
				mLODIndex = real(MaxIndex);
			}
		}

		/// Get the distance from the camera to the model's bounding sphere		
		/// surface, divided by the bounding sphere size								
		///	@return the distance																
		real GetNormalizedDistance() const noexcept {
			return mDistanceToSurface / mRadius;
		}

		/// Return the LOD index, which is a real number in the range				
		/// [MinIndex;MaxIndex]																	
		/// Calculate the LOD index via log10 distance from a sphere to			
		/// the camera view. You can imagine it as the number of zeroes behind	
		/// or in front of the distance														
		/// If index is below zero, then we're observing from afar					
		/// If index is above zero, then we're observing too close					
		/// At zero we're observing the default quality asset							
		///	@return the LOD index															
		real GetIndex() const noexcept {
			return mLODIndex;
		}

		/// Get LOD index in the range [0;IndexCount)									
		///	@return the absolute index														
		AbsoluteLodIndex GetAbsoluteIndex() const noexcept {
			return AbsoluteLodIndex(LodIndex(mLODIndex) - MinIndex);
		}
	};


	///																								
	///  ABSTRACT GEOMETRY INTERFACE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AGeometry : public AContent {
	protected:
		VertexView mView;
		Mapper mTextureMapper = Mapper::Model;
		pcptr mTesselation = 0;

	public:
		REFLECT(AGeometry);
		using AContent::AContent;

		virtual const AGeometry* GetLOD(const LodState&) const = 0;

		NOD() virtual pcptr GetLineCount() const = 0;
		NOD() virtual vec2u GetLineIndices(pcptr) const = 0;
		NOD() virtual Line3 GetLine(pcptr) const = 0;
		NOD() virtual Any GetLineTrait(pcptr, TMeta) const = 0;

		NOD() virtual pcptr GetTriangleCount() const = 0;
		NOD() virtual vec3u GetTriangleIndices(pcptr) const = 0;
		NOD() virtual Triangle3 GetTriangle(pcptr) const = 0;
		NOD() virtual Any GetTriangleTrait(pcptr, TMeta) const = 0;

		void SetTextureMapper(Mapper);
		NOD() Mapper GetTextureMapper() const noexcept;

		void SetTesselation(pcptr);
		NOD() pcptr GetTesselation() const noexcept;

		void SetBilaterality(bool);
		NOD() bool GetBilaterality() const noexcept;

		void SetTopology(DMeta);

		template<RTTI::ReflectedData T>
		void SetTopology();

		NOD() DMeta GetTopology() const noexcept;

		template<RTTI::ReflectedData T>
		NOD() bool CheckTopology() const;

		template<RTTI::ReflectedData T>
		NOD() bool IsTopology() const;

		PC_SET(View);
	};


	///																								
	///  ABSTRACT TEXTURE INTERFACE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ATexture : public AContent {
	protected:
		PixelView mView;

	public:
		REFLECT(ATexture);
		using AContent::AContent;

		PC_SET(View);
		NOD() inline auto& GetFormat() const noexcept { return mView.mFormat; }
		NOD() inline auto& GetWidth() const noexcept { return mView.mWidth; }
		NOD() inline auto& GetHeight() const noexcept { return mView.mHeight; }
	};


	///																								
	///  ABSTRACT FONT INTERFACE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AFont : public ATexture {
	public:
		using CodePoint = pcu32;
		using Kerning = float;
		using Locale = Text;

		inline static const Locale DefaultLocale = "";
		inline static const Path DefaultFont = "AnkaCoder-r";

		static constexpr pcptr GlyphCount = 256;
		static constexpr pcu32 DefaultScale = 64;
		static constexpr pcu32 DefaultSpreadInPixels = 128;
		static constexpr pcu32 ScaleFactor = DefaultSpreadInPixels / DefaultScale;
		static_assert(DefaultSpreadInPixels% DefaultScale == 0, "Sizes must be divisible");

		///																							
		///	GLYPH																					
		///																							
		struct Glyph {
			Glyph() = default;

			CodePoint mCodePoint = 0;
			vec4i16 mRect;
			vec2i16 mSize;
			vec3i16 mHorizontalBearingAndAdvance;
			vec3i16 mVerticalBearingAndAdvance;
			TMap<CodePoint, pci16> mKernings;
		};

	protected:
		Text mFontName;
		Locale mLocale = DefaultLocale;
		Glyph mGlyphs[GlyphCount];

	public:
		REFLECT(AFont);
		using ATexture::ATexture;

		PC_SET(FontName);
		PC_SET(Locale);

		NOD() inline auto& GetGlyph(char c) const noexcept {
			return mGlyphs[c];
		}
	};


	///																								
	///  ABSTRACT SOUND INTERFACE																
	///																								
	class LANGULUS_MODULE(FRAMEWORK) ASound : public AContent {
	protected:
		SoundView mView;

	public:
		REFLECT(ASound);
		using AContent::AContent;

		PC_SET(View);
	};


	///																								
	///  ABSTRACT MATERIAL INTERFACE															
	///																								
	class LANGULUS_MODULE(FRAMEWORK) AMaterial : public AContent {
	protected:
		MaterialView mView;
		RRate mDefaultRate = RRate::PerAuto;
		GASM mCode;

	public:
		REFLECT(AMaterial);
		using AContent::AContent;

		PC_SET(DefaultRate);
		PC_SET(View);
	};

} // namespace PCFW

/// Declare constants for use in GASM and RTTI											
LANGULUS_DECLARE_CONSTANT(PerAuto, RRate(RRate::PerAuto), 
	"Automatically determined refresh rate");
LANGULUS_DECLARE_CONSTANT(PerNone, RRate(RRate::PerNone), 
	"Compile-time refresh rate (essentially a constant)");
LANGULUS_DECLARE_CONSTANT(PerTick, RRate(RRate::PerTick), 
	"Refresh once per tick (when time moves)");
LANGULUS_DECLARE_CONSTANT(PerPass, RRate(RRate::PerPass), 
	"Refresh once per draw pass");
LANGULUS_DECLARE_CONSTANT(PerCamera, RRate(RRate::PerCamera), 
	"Refresh once per camera");
LANGULUS_DECLARE_CONSTANT(PerLevel, RRate(RRate::PerLevel), 
	"Refresh once per octave scale");
LANGULUS_DECLARE_CONSTANT(PerRenderable, RRate(RRate::PerRenderable), 
	"Refresh once per renderable object");
LANGULUS_DECLARE_CONSTANT(PerInstance, RRate(RRate::PerInstance), 
	"Refresh once per instance");
LANGULUS_DECLARE_CONSTANT(PerVertex, RRate(RRate::PerVertex), 
	"Refresh once per vertex (inside vertex shader)");
LANGULUS_DECLARE_CONSTANT(PerPrimitive, RRate(RRate::PerPrimitive), 
	"Refresh once per geometric primitive (inside geometry shader)");
LANGULUS_DECLARE_CONSTANT(PerPixel, RRate(RRate::PerPixel), 
	"Refresh once per pixel (inside fragment shader)");

LANGULUS_DECLARE_CONSTANT(WorldMapper, Mapper(Mapper::World),
	"Maps to transformed model's world vertex positions, calculated at realtime in shaders");
LANGULUS_DECLARE_CONSTANT(ModelMapper, Mapper(Mapper::Model),
	"Maps to 3D vertex positions, either precalculated or done at realtime in shaders");
LANGULUS_DECLARE_CONSTANT(ProjectorMapper, Mapper(Mapper::Projector),
	"Maps to a 2D camera-like projection, calculated at realtime in shaders");
LANGULUS_DECLARE_CONSTANT(ScreenMapper, Mapper(Mapper::Screen),
	"Maps to 2D screen coordinates, calculated at realtime in shaders");
LANGULUS_DECLARE_CONSTANT(PlaneMapper, Mapper(Mapper::Plane),
	"Maps to a 2D planar projection");
LANGULUS_DECLARE_CONSTANT(CylinderMapper, Mapper(Mapper::Cylinder),
	"Maps to a 2D cylindrical projection");
LANGULUS_DECLARE_CONSTANT(SphereMapper, Mapper(Mapper::Sphere),
	"Maps to a 2D spherical projection with sharp warping at the poles");
LANGULUS_DECLARE_CONSTANT(CubicMapper, Mapper(Mapper::Cubic),
	"Maps to a 2D cubic projection (useful for skyboxes)");
LANGULUS_DECLARE_CONSTANT(FaceMapper, Mapper(Mapper::Face),
	"Maps each face to the same 2D texture coordinates");
LANGULUS_DECLARE_CONSTANT(InjectMapper, Mapper(Mapper::Inject),
	"Maps each 2D face individually at generation time, preserving the relative scale, but causing seams all over the place");
LANGULUS_DECLARE_CONSTANT(ContourMapper, Mapper(Mapper::Contour),
	"Maps 2D along an edge, useful for texturing roads, etc.");
LANGULUS_DECLARE_CONSTANT(UnfoldMapper, Mapper(Mapper::Unfold),
	"Unwraps vertex positions along seams and stretches them like a 2D pelt");
LANGULUS_DECLARE_CONSTANT(CustomMapper, Mapper(Mapper::Custom),
	"No mapping is done - you truly trust the raw data, even if it is invalid or non-existent");

LANGULUS_DECLARE_CONSTANT(NoBlend, BlendMode(BlendMode::None),
	"No blending - final color is always source color");
LANGULUS_DECLARE_CONSTANT(ThresholdBlend, BlendMode(BlendMode::Threshold),
	"No blending - final color becomes source color only if an alpha threshold is reached per pixel");
LANGULUS_DECLARE_CONSTANT(NormalBlend, BlendMode(BlendMode::Normal),
	"Final color = source color * alpha + destination color * (1 - alpha)");
LANGULUS_DECLARE_CONSTANT(AddBlend, BlendMode(BlendMode::Add),
	"Final color = source color + destination color");
LANGULUS_DECLARE_CONSTANT(MultiplyBlend, BlendMode(BlendMode::Multiply),
	"Final color = source color * destination color");

/// Declare traits for use in GASM and RTTI												
LANGULUS_DECLARE_TRAIT_FILTERED(TextureMapper, "Texture mapper trait", DataID::Of<Mapper>);
LANGULUS_DECLARE_TRAIT(Resolution, "Resolution trait");
LANGULUS_DECLARE_TRAIT(Tesselation, "Tesselation trait");
LANGULUS_DECLARE_TRAIT(Topology, "Topology trait");
LANGULUS_DECLARE_TRAIT(Interpolator, "Interpolator trait");

/// Declare exceptions																			
LANGULUS_DECLARE_EXCEPTION(Content);
LANGULUS_DECLARE_EXCEPTION(MissingTrait);

#include "AModuleContent.inl"
