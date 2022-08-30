#include "AModulePlatform.hpp"
#include "AModuleGraphics.hpp"

namespace PCFW
{

	/// Default window construction															
	///	@param classid - type of the actual window component						
	AWindow::AWindow(DMeta classid)
		: AUnitPlatform {classid} {}

	/// Move-construct a window																
	///	@param other - the window to move												
	AWindow::AWindow(AWindow&& other) noexcept
		: AUnitPlatform {pcForward<AUnitPlatform>(other)}
		, mRenderer {pcMove(other.mRenderer)}
		, mNativeWindowHandle {pcMove(other.mNativeWindowHandle)}
		, mSize {other.mSize}
		, mTitle {pcMove(other.mTitle)}
		, mMousePosition {other.mMousePosition}
		, mMouseScroll {other.mMouseScroll}
		, mTextInput {pcMove(other.mTextInput)} {
		if (mRenderer) {
			// Make sure to notify the renderer of the move						
			// Otherwise bad things will happen										
			mRenderer->SetWindow(this);
		}
	}

	/// Move-copy a window																		
	///	@param other - the window to move												
	AWindow& AWindow::operator = (AWindow&& other) noexcept {
		AUnitPlatform::operator = (pcForward<AUnitPlatform>(other));
		mRenderer = pcMove(other.mRenderer);
		mNativeWindowHandle = pcMove(other.mNativeWindowHandle);
		mSize = other.mSize;
		mTitle = pcMove(other.mTitle);
		mMousePosition = other.mMousePosition;
		mMouseScroll = other.mMouseScroll;
		mTextInput = pcMove(other.mTextInput);
		if (mRenderer) {
			// Make sure to notify the renderer of the move						
			// Otherwise bad things will happen										
			mRenderer->SetWindow(this);
		}
		return *this;
	}

} // namespace PCFW