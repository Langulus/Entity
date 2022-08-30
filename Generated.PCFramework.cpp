#include "include/PCFW.hpp"

namespace PCFW
{
	
	REFLECT_BEGIN(CState)
		REFLECT_INFO("A context state")
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END

	REFLECT_BEGIN(AContext)
		REFLECT_INFO("Abstract context")
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM,Debug)
		REFLECT_END_ABILITIES
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mClassState, State)
		REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(Entity)
		REFLECT_INFO("An entity")
		REFLECT_PRODUCER(Entity)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContext)
		REFLECT_END_BASES
		REFLECT_BEGIN_ABILITIES
			REFLECT_ABILITY(Do),
			REFLECT_ABILITY(Select),
			REFLECT_ABILITY(Create),
			REFLECT_CONVERSIONS(GASM,Debug)
		REFLECT_END_ABILITIES
		/*REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mOwner, Owner),
			REFLECT_MEMBER_TRAIT(mChildren, Child),
			REFLECT_MEMBER_TRAIT(mUnits, Unit),
			REFLECT_MEMBER_TRAIT(mTraits, Trait)
		REFLECT_END_MEMBERS*/
	REFLECT_END

	REFLECT_BEGIN(AUnit)
		REFLECT_INFO("An abstract unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContext),
			REFLECT_BASE(Referenced)
		REFLECT_END_BASES
		REFLECT_BEGIN_ABILITIES
			REFLECT_ABILITY(Select)
		REFLECT_END_ABILITIES
	REFLECT_END

	REFLECT_BEGIN(CFlow)
		REFLECT_INFO("A GASM flow")
		REFLECT_PRODUCER(Entity)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mTimeFromInit, Time)
		REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(CRuntime)
		REFLECT_INFO("A system component")
		REFLECT_PRODUCER(Entity)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END


	///																								
	/// MODULES																						
	///																								
	REFLECT_BEGIN(AModule)
		REFLECT_INFO("An abstract module")
		REFLECT_PRODUCER(CRuntime)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContext)
		REFLECT_END_BASES
		//REFLECT_BEGIN_MEMBERS
		//	REFLECT_MEMBER_TRAIT(mRuntime, System)
		//REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(AModuleAI)
		REFLECT_INFO("An abstract AIAD module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleAudio)
		REFLECT_INFO("An abstract audio module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModulePlatform)
		REFLECT_INFO("An abstract platform module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleGraphics)
		REFLECT_INFO("An abstract graphics module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleUMS)
		REFLECT_INFO("An abstract platform module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModulePhysics)
		REFLECT_INFO("An abstract physics module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleOrganics)
		REFLECT_INFO("An abstract organics module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleNetwork)
		REFLECT_INFO("An abstract network module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleMath)
		REFLECT_INFO("An abstract math module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleLogger)
		REFLECT_INFO("An abstract log module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleInput)
		REFLECT_INFO("An abstract input module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleHardware)
		REFLECT_INFO("An abstract hardware module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleUI)
		REFLECT_INFO("An abstract UI module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleEconomy)
		REFLECT_INFO("An abstract economy module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleData)
		REFLECT_INFO("An abstract data module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleContent)
		REFLECT_INFO("An abstract content module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AModuleVFS)
		REFLECT_INFO("An abstract virtual file system module")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AModule)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(Event)
		REFLECT_INFO("An input event")
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mEvent, Type),
			REFLECT_MEMBER_TRAIT(mState, State)
		REFLECT_END_MEMBERS
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END


	///																								
	/// UNITS																						
	///																								
	REFLECT_BEGIN(AUnitAI)
		REFLECT_INFO("An abstract AIAD unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitPlatform)
		REFLECT_INFO("An abstract platform unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AWindow)
		REFLECT_INFO("An abstract window unit")
		REFLECT_PRODUCER(AModulePlatform)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitPlatform)
		REFLECT_END_BASES
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mSize, Scale),
			REFLECT_MEMBER_TRAIT(mTitle, Name),
			REFLECT_MEMBER_TRAIT(mMousePosition, MousePosition),
			REFLECT_MEMBER_TRAIT(mMouseScroll, MouseScroll)
		REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(AUnitGraphics)
		REFLECT_INFO("An abstract graphics unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(ARenderer)
		REFLECT_INFO("An abstract renderer unit")
		REFLECT_PRODUCER(AModuleGraphics)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitGraphics)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AVisualLayer)
		REFLECT_INFO("An abstract visual layer unit")
		REFLECT_PRODUCER(ARenderer)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitGraphics)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(ARenderable)
		REFLECT_INFO("An abstract renderable unit")
		REFLECT_PRODUCER(AVisualLayer)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitGraphics)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(ACamera)
		REFLECT_INFO("An abstract camera unit")
		REFLECT_PRODUCER(AVisualLayer)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitGraphics)
		REFLECT_END_BASES
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mPerspective, Perspective),
			REFLECT_MEMBER_TRAIT(mProjection, ProjectTransform),
			REFLECT_MEMBER_TRAIT(mViewport, Viewport),
			REFLECT_MEMBER_TRAIT(mFOV, FOV),
			REFLECT_MEMBER_TRAIT(mAspectRatio, AspectRatio),
			REFLECT_MEMBER_TRAIT(mObservableRange, Level),
			REFLECT_MEMBER(mEyeSeparation)
		REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(ALight)
		REFLECT_INFO("An abstract light unit")
		REFLECT_PRODUCER(AVisualLayer)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitGraphics)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(IContentVRAM)
		REFLECT_INFO("An abstract VRAM content")
		REFLECT_PRODUCER(ARenderer)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitGraphics)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitUMS)
		REFLECT_INFO("An abstract platform unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitPhysics)
		REFLECT_INFO("An abstract physics unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUniverse)
		REFLECT_INFO("An abstract universe unit")
		REFLECT_PRODUCER(AModulePhysics)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitPhysics)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AInstance)
		REFLECT_INFO("An abstract instance unit")
		REFLECT_PRODUCER(AUniverse)
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitPhysics),
			REFLECT_BASE(InstanceType)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitOrganics)
		REFLECT_INFO("An abstract organics unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitNetwork)
		REFLECT_INFO("An abstract network unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitMath)
		REFLECT_INFO("An abstract math unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitInput)
		REFLECT_INFO("An abstract input unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitHardware)
		REFLECT_INFO("An abstract hardware unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitUI)
		REFLECT_INFO("An abstract UI unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitEconomy)
		REFLECT_INFO("An abstract economy unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AContent)
		REFLECT_PRODUCER(AModuleContent)
		REFLECT_INFO("An abstract content unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AGeometry)
		REFLECT_PRODUCER(AModuleContent)
		REFLECT_INFO("An abstract geometry unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContent)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(ATexture)
		REFLECT_PRODUCER(AModuleContent)
		REFLECT_INFO("An abstract texture unit")
		REFLECT_FILE("png")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContent)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AFont)
		REFLECT_PRODUCER(AModuleContent)
		REFLECT_INFO("An abstract font unit")
		REFLECT_FILE("ttf,pcf")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContent)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(ASound)
		REFLECT_PRODUCER(AModuleContent)
		REFLECT_INFO("An abstract sound unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContent)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AMaterial)
		REFLECT_PRODUCER(AModuleContent)
		REFLECT_INFO("An abstract material unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AContent)
		REFLECT_END_BASES
	REFLECT_BEGIN_MEMBERS
		REFLECT_MEMBER_TRAIT(mDefaultRate, Frequency),
		REFLECT_MEMBER_TRAIT(mCode, Code)
	REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(AUnitAudio)
		REFLECT_INFO("An abstract audio unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AUnitVFS)
		REFLECT_INFO("An abstract virtual file system unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnit)
		REFLECT_END_BASES
	REFLECT_END

	REFLECT_BEGIN(AFile)
		REFLECT_PRODUCER(AModuleVFS)
		REFLECT_INFO("An abstract file unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitVFS)
		REFLECT_END_BASES
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mFilename, Name),
			REFLECT_MEMBER_TRAIT(mFormat, Type),
			REFLECT_MEMBER_TRAIT(mByteCount, Count),
			REFLECT_MEMBER_TRAIT(mState, State)
		REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(AFolder)
		REFLECT_PRODUCER(AModuleVFS)
		REFLECT_INFO("An abstract folder unit")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(AUnitVFS)
		REFLECT_END_BASES
		REFLECT_BEGIN_MEMBERS
			REFLECT_MEMBER_TRAIT(mDirname, Name),
			REFLECT_MEMBER_TRAIT(mState, State)
		REFLECT_END_MEMBERS
	REFLECT_END

	REFLECT_BEGIN(EState)
		REFLECT_INFO("An event state")
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END

	REFLECT_BEGIN(FState)
		REFLECT_INFO("A file state")
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END
			
	REFLECT_BEGIN(RRate)
		REFLECT_INFO("A refresh rate")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(std::underlying_type_t<RRate::Enum>)
		REFLECT_END_BASES
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END
			
	REFLECT_BEGIN(BlendMode)
		REFLECT_INFO("A blending mode")
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END
			
	REFLECT_BEGIN(Mapper)
		REFLECT_INFO("A mapper state")
		REFLECT_BEGIN_ABILITIES
			REFLECT_CONVERSIONS(GASM)
		REFLECT_END_ABILITIES
	REFLECT_END
			
	REFLECT_BEGIN(GLSL)
		REFLECT_INFO("GLSL code text container")
		//REFLECT_FILE("glsl,frag,vert,geom,comp,tesse,tessc")
		REFLECT_BEGIN_BASES
			REFLECT_BASE(ACode),
			REFLECT_BASE(Text)
		REFLECT_END_BASES
	REFLECT_END


	template<RTTI::ReflectedData HEAD, RTTI::ReflectedData... TAIL>
	void VectorToGLSLConverters() {
		auto meta = MetaData::LOCK<HEAD>();
		meta->AddAbility(REFLECT_CONVERSIONS_EXT(HEAD, GLSL));
		if constexpr (sizeof...(TAIL) > 0)
			VectorToGLSLConverters<TAIL...>();
	}

	template<RTTI::ReflectedData... T>
	void VectorToGLSLConverters(const TTypeList<T...>&) {
		VectorToGLSLConverters<T...>();
	}

	bool pcInitFrameworkMeta() {
		static bool guard = false;
		if (guard)
			return true;
		guard = true;

		// Base contexts																	
		MetaData::REGISTER<AContext>();
		MetaData::REGISTER<AModule>();
		MetaData::REGISTER<AUnit>();
		MetaData::REGISTER<AContent>();

		// Containers																		
		MetaData::REGISTER<Entity>();

		// Components																		
		MetaData::REGISTER<CFlow>();
		MetaData::REGISTER<CRuntime>();

		// Module interfaces																
		MetaData::REGISTER<AModuleAI>();
		MetaData::REGISTER<AModuleAudio>();
		MetaData::REGISTER<AModuleContent>();
		MetaData::REGISTER<AModuleData>();
		MetaData::REGISTER<AModuleNetwork>();
		MetaData::REGISTER<AModuleUI>();
		MetaData::REGISTER<AModuleHardware>();
		MetaData::REGISTER<AModuleInput>();
		MetaData::REGISTER<AModuleLogger>();
		MetaData::REGISTER<AModuleMath>();
		MetaData::REGISTER<AModuleEconomy>();
		MetaData::REGISTER<AModuleGraphics>();
		MetaData::REGISTER<AModuleOrganics>();
		MetaData::REGISTER<AModulePhysics>();
		MetaData::REGISTER<AModulePlatform>();
		MetaData::REGISTER<AModuleUMS>();
		MetaData::REGISTER<AModuleVFS>();

		// AUnit interfaces																
		MetaData::REGISTER<AUnitAI>();
		MetaData::REGISTER<AUnitAudio>();
		MetaData::REGISTER<ATexture>();
		MetaData::REGISTER<AFont>();
		MetaData::REGISTER<AMaterial>();
		MetaData::REGISTER<AGeometry>();
		MetaData::REGISTER<ASound>();
		MetaData::REGISTER<AUnitNetwork>();
		MetaData::REGISTER<AUnitUI>();
		MetaData::REGISTER<AUnitHardware>();
		MetaData::REGISTER<AUnitInput>();
		MetaData::REGISTER<Event>();
		MetaData::REGISTER<AUnitMath>();
		MetaData::REGISTER<AUnitEconomy>();
		MetaData::REGISTER<AUnitGraphics>();
		MetaData::REGISTER<ARenderer>();
		MetaData::REGISTER<AVisualLayer>();
		MetaData::REGISTER<ARenderable>();
		MetaData::REGISTER<ACamera>();
		MetaData::REGISTER<ALight>();
		MetaData::REGISTER<IContentVRAM>();
		MetaData::REGISTER<AUnitOrganics>();
		MetaData::REGISTER<AUnitPhysics>();
		MetaData::REGISTER<AUniverse>();
		MetaData::REGISTER<AInstance>();
		MetaData::REGISTER<AUnitPlatform>();
		MetaData::REGISTER<AWindow>();
		MetaData::REGISTER<AUnitUMS>();
		MetaData::REGISTER<AUnitVFS>();
		MetaData::REGISTER<AFile>();
		MetaData::REGISTER<AFolder>();
		MetaData::REGISTER<GLSL>();

		// States																			
		MetaData::REGISTER<CState>();
		MetaData::REGISTER<EState>();
		MetaData::REGISTER<FState>();
		MetaData::REGISTER<Mapper>();

		// Traits																			
		MetaTrait::REGISTER<Traits::Perspective>();
		MetaTrait::REGISTER<Traits::FOV>();
		MetaTrait::REGISTER<Traits::AspectRatio>();
		MetaTrait::REGISTER<Traits::Viewport>();

		MetaTrait::REGISTER<Traits::ViewTransform>();
		MetaTrait::REGISTER<Traits::ProjectTransform>();
		MetaTrait::REGISTER<Traits::ViewProjectTransform>();
		MetaTrait::REGISTER<Traits::ModelViewProjectTransform>();

		MetaTrait::REGISTER<Traits::ViewTransformInverted>();
		MetaTrait::REGISTER<Traits::ProjectTransformInverted>();
		MetaTrait::REGISTER<Traits::ViewProjectTransformInverted>();
		MetaTrait::REGISTER<Traits::ModelViewProjectTransformInverted>();

		MetaTrait::REGISTER<Traits::Color>();
		MetaTrait::REGISTER<Traits::Material>();
		MetaTrait::REGISTER<Traits::Model>();
		MetaTrait::REGISTER<Traits::Texture>();

		MetaTrait::REGISTER<Traits::Resolution>();
		MetaTrait::REGISTER<Traits::Tesselation>();
		MetaTrait::REGISTER<Traits::Topology>();
		MetaTrait::REGISTER<Traits::Interpolator>();
		MetaTrait::REGISTER<Traits::TextureMapper>();

		MetaTrait::REGISTER<Traits::MousePosition>();
		MetaTrait::REGISTER<Traits::MouseScroll>();

		// Constants																		
		MetaConst::REGISTER<Constants::AnyES>();
		MetaConst::REGISTER<Constants::ActivatedES>();
		MetaConst::REGISTER<Constants::DeactivatedES>();
		MetaConst::REGISTER<Constants::WasActiveES>();
		MetaConst::REGISTER<Constants::DisabledES>();
		MetaConst::REGISTER<Constants::LockedES>();
		MetaConst::REGISTER<Constants::DoubleClickES>();
		MetaConst::REGISTER<Constants::RepeatES>();
		MetaConst::REGISTER<Constants::ReleasedES>();

		MetaConst::REGISTER<Constants::AutoFS>();
		MetaConst::REGISTER<Constants::ReadFS>();
		MetaConst::REGISTER<Constants::WriteFS>();
		MetaConst::REGISTER<Constants::AppendFS>();
		MetaConst::REGISTER<Constants::CreateFS>();
		MetaConst::REGISTER<Constants::BinaryFS>();

		MetaConst::REGISTER<Constants::DefaultCS>();
		MetaConst::REGISTER<Constants::InvalidCS>();
		MetaConst::REGISTER<Constants::ConstantCS>();
		MetaConst::REGISTER<Constants::SharedCS>();
		MetaConst::REGISTER<Constants::DynamicCS>();
		MetaConst::REGISTER<Constants::DisabledCS>();
		MetaConst::REGISTER<Constants::BusyCS>();
		MetaConst::REGISTER<Constants::PersistentCS>();

		MetaConst::REGISTER<Constants::WorldMapper>();
		MetaConst::REGISTER<Constants::ModelMapper>();
		MetaConst::REGISTER<Constants::ProjectorMapper>();
		MetaConst::REGISTER<Constants::ScreenMapper>();
		MetaConst::REGISTER<Constants::PlaneMapper>();
		MetaConst::REGISTER<Constants::CylinderMapper>();
		MetaConst::REGISTER<Constants::SphereMapper>();
		MetaConst::REGISTER<Constants::CubicMapper>();
		MetaConst::REGISTER<Constants::FaceMapper>();
		MetaConst::REGISTER<Constants::InjectMapper>();
		MetaConst::REGISTER<Constants::ContourMapper>();
		MetaConst::REGISTER<Constants::UnfoldMapper>();
		MetaConst::REGISTER<Constants::CustomMapper>();

		MetaConst::REGISTER<Constants::NoBlend>();
		MetaConst::REGISTER<Constants::ThresholdBlend>();
		MetaConst::REGISTER<Constants::NormalBlend>();
		MetaConst::REGISTER<Constants::AddBlend>();
		MetaConst::REGISTER<Constants::MultiplyBlend>();

		MetaConst::REGISTER<Constants::PerNone>();
		MetaConst::REGISTER<Constants::PerTick>();
		MetaConst::REGISTER<Constants::PerPass>();
		MetaConst::REGISTER<Constants::PerCamera>();
		MetaConst::REGISTER<Constants::PerLevel>();
		MetaConst::REGISTER<Constants::PerRenderable>();
		MetaConst::REGISTER<Constants::PerInstance>();
		MetaConst::REGISTER<Constants::PerVertex>();
		MetaConst::REGISTER<Constants::PerPrimitive>();
		MetaConst::REGISTER<Constants::PerPixel>();

		// Register some standard events												
		MetaData::REGISTER<Events::Ok>();
		MetaData::REGISTER<Events::Verbose>();
		MetaData::REGISTER<Events::Info>();
		MetaData::REGISTER<Events::Error>();
		MetaData::REGISTER<Events::Warning>();
		MetaData::REGISTER<Events::Message>();
		MetaData::REGISTER<Events::Special>();
		MetaData::REGISTER<Events::Flow>();
		MetaData::REGISTER<Events::Input>();
		MetaData::REGISTER<Events::TCP>();
		MetaData::REGISTER<Events::UDP>();
		MetaData::REGISTER<Events::OS>();
		MetaData::REGISTER<Events::Choice>();

		MetaData::REGISTER<Keys::Key0>();
		MetaData::REGISTER<Keys::Key1>();
		MetaData::REGISTER<Keys::Key2>();
		MetaData::REGISTER<Keys::Key3>();
		MetaData::REGISTER<Keys::Key4>();
		MetaData::REGISTER<Keys::Key5>();
		MetaData::REGISTER<Keys::Key6>();
		MetaData::REGISTER<Keys::Key7>();
		MetaData::REGISTER<Keys::Key8>();
		MetaData::REGISTER<Keys::Key9>();

		MetaData::REGISTER<Keys::A>();
		MetaData::REGISTER<Keys::B>();
		MetaData::REGISTER<Keys::C>();
		MetaData::REGISTER<Keys::D>();
		MetaData::REGISTER<Keys::E>();
		MetaData::REGISTER<Keys::F>();
		MetaData::REGISTER<Keys::G>();
		MetaData::REGISTER<Keys::H>();
		MetaData::REGISTER<Keys::I>();
		MetaData::REGISTER<Keys::J>();
		MetaData::REGISTER<Keys::K>();
		MetaData::REGISTER<Keys::L>();
		MetaData::REGISTER<Keys::M>();
		MetaData::REGISTER<Keys::N>();
		MetaData::REGISTER<Keys::O>();
		MetaData::REGISTER<Keys::P>();
		MetaData::REGISTER<Keys::Q>();
		MetaData::REGISTER<Keys::R>();
		MetaData::REGISTER<Keys::S>();
		MetaData::REGISTER<Keys::T>();
		MetaData::REGISTER<Keys::U>();
		MetaData::REGISTER<Keys::V>();
		MetaData::REGISTER<Keys::W>();
		MetaData::REGISTER<Keys::X>();
		MetaData::REGISTER<Keys::Y>();
		MetaData::REGISTER<Keys::Z>();

		MetaData::REGISTER<Keys::ScrollLock>();
		MetaData::REGISTER<Keys::CapsLock>();

		MetaData::REGISTER<Keys::Left>();
		MetaData::REGISTER<Keys::Down>();
		MetaData::REGISTER<Keys::Up>();
		MetaData::REGISTER<Keys::Right>();

		MetaData::REGISTER<Keys::F1>();
		MetaData::REGISTER<Keys::F2>();
		MetaData::REGISTER<Keys::F3>();
		MetaData::REGISTER<Keys::F4>();
		MetaData::REGISTER<Keys::F5>();
		MetaData::REGISTER<Keys::F6>();
		MetaData::REGISTER<Keys::F7>();
		MetaData::REGISTER<Keys::F8>();
		MetaData::REGISTER<Keys::F9>();
		MetaData::REGISTER<Keys::F10>();
		MetaData::REGISTER<Keys::F11>();
		MetaData::REGISTER<Keys::F12>();
		MetaData::REGISTER<Keys::F13>();
		MetaData::REGISTER<Keys::F14>();
		MetaData::REGISTER<Keys::F15>();
		MetaData::REGISTER<Keys::F16>();
		MetaData::REGISTER<Keys::F17>();
		MetaData::REGISTER<Keys::F18>();
		MetaData::REGISTER<Keys::F19>();
		MetaData::REGISTER<Keys::F20>();
		MetaData::REGISTER<Keys::F21>();
		MetaData::REGISTER<Keys::F22>();
		MetaData::REGISTER<Keys::F23>();
		MetaData::REGISTER<Keys::F24>();

		MetaData::REGISTER<Keys::Joystick1>();
		MetaData::REGISTER<Keys::Joystick2>();
		MetaData::REGISTER<Keys::Joystick3>();
		MetaData::REGISTER<Keys::Joystick4>();
		MetaData::REGISTER<Keys::Joystick5>();
		MetaData::REGISTER<Keys::Joystick6>();
		MetaData::REGISTER<Keys::Joystick7>();
		MetaData::REGISTER<Keys::Joystick8>();
		MetaData::REGISTER<Keys::Joystick9>();
		MetaData::REGISTER<Keys::Joystick10>();
		MetaData::REGISTER<Keys::Joystick11>();
		MetaData::REGISTER<Keys::Joystick12>();
		MetaData::REGISTER<Keys::Joystick13>();
		MetaData::REGISTER<Keys::Joystick14>();
		MetaData::REGISTER<Keys::Joystick15>();
		MetaData::REGISTER<Keys::Joystick16>();

		MetaData::REGISTER<Keys::LAlt>();
		MetaData::REGISTER<Keys::LShift>();
		MetaData::REGISTER<Keys::LControl>();

		MetaData::REGISTER<Keys::RAlt>();
		MetaData::REGISTER<Keys::RShift>();
		MetaData::REGISTER<Keys::RControl>();

		MetaData::REGISTER<Keys::MouseLeft>();
		MetaData::REGISTER<Keys::MouseMiddle>();
		MetaData::REGISTER<Keys::MouseRight>();
		MetaData::REGISTER<Keys::Mouse4>();
		MetaData::REGISTER<Keys::Mouse5>();
		MetaData::REGISTER<Keys::Mouse6>();
		MetaData::REGISTER<Keys::Mouse7>();
		MetaData::REGISTER<Keys::Mouse8>();
		MetaData::REGISTER<Keys::MouseMoveHorizontal>();
		MetaData::REGISTER<Keys::MouseMoveVertical>();
		MetaData::REGISTER<Keys::MouseScrollHorizontal>();
		MetaData::REGISTER<Keys::MouseScrollVertical>();

		MetaData::REGISTER<Keys::Num0>();
		MetaData::REGISTER<Keys::Num1>();
		MetaData::REGISTER<Keys::Num2>();
		MetaData::REGISTER<Keys::Num3>();
		MetaData::REGISTER<Keys::Num4>();
		MetaData::REGISTER<Keys::Num5>();
		MetaData::REGISTER<Keys::Num6>();
		MetaData::REGISTER<Keys::Num7>();
		MetaData::REGISTER<Keys::Num8>();
		MetaData::REGISTER<Keys::Num9>();
		MetaData::REGISTER<Keys::NumAdd>();
		MetaData::REGISTER<Keys::NumDecimal>();
		MetaData::REGISTER<Keys::NumDivide>();
		MetaData::REGISTER<Keys::NumLock>();
		MetaData::REGISTER<Keys::NumMultiply>();
		MetaData::REGISTER<Keys::NumpadEqual>();
		MetaData::REGISTER<Keys::NumSubtract>();

		MetaData::REGISTER<Keys::Enter>();
		MetaData::REGISTER<Keys::Escape>();
		MetaData::REGISTER<Keys::Back>();
		MetaData::REGISTER<Keys::Home>();
		MetaData::REGISTER<Keys::Insert>();
		MetaData::REGISTER<Keys::PageDown>();
		MetaData::REGISTER<Keys::PageUp>();
		MetaData::REGISTER<Keys::Delete>();
		MetaData::REGISTER<Keys::End>();
		MetaData::REGISTER<Keys::Pause>();
		MetaData::REGISTER<Keys::Print>();

		MetaData::REGISTER<Keys::Period>();
		MetaData::REGISTER<Keys::Quote>();
		MetaData::REGISTER<Keys::Semicolon>();
		MetaData::REGISTER<Keys::Slash>();
		MetaData::REGISTER<Keys::Space>();
		MetaData::REGISTER<Keys::Tab>();
		MetaData::REGISTER<Keys::Tilde>();
		MetaData::REGISTER<Keys::Minus>();
		MetaData::REGISTER<Keys::LeftBracket>();
		MetaData::REGISTER<Keys::RightBracket>();
		MetaData::REGISTER<Keys::Hack>();
		MetaData::REGISTER<Keys::Comma>();

		// Integrate math primitives to GLSL										
		VectorToGLSLConverters(Math::TColorTypes{});
		VectorToGLSLConverters(Math::TVecTypes{});
		VectorToGLSLConverters(Math::TMatrixTypes{});
		return true;
	}

} // namespace PCFW
