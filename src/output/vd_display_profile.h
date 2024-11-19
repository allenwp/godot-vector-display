#ifndef VD_DISPLAY_PROFILE_H
#define VD_DISPLAY_PROFILE_H

#include "vd_sample.h"

namespace vector_display {

class VDDisplayProfile {
public:
	/// <summary>
	/// To calculate this, view the Calibration Squares screen and:
	/// 1) Adjust the height to be correct for the screen
	/// 2) Adjust the width to make the image square.
	/// 3) Adjust the Aspect ratio and repeat above until step 2 gives the correct width for the screen.
	/// </summary>
	float AspectRatio = 1.0f;

	/// <summary>
	/// VDSample Brightness of 1.0 will be adjusted to this output value.
	/// Usually something like -1.0 for an oscilloscope.
	/// </summary>
	float FullBrightnessOutput = -1.0f;

	/// <summary>
	/// VDSample Brightness of 0.0 will be adjusted to this output value.
	/// Usually something like 1.0 for an oscilloscope.
	/// </summary>
	float ZeroBrightnessOutput = 1.0f;

	/// <summary>
	/// If the display doesn't support scaling X and Y input, this can be used
	/// to scale the output to the visible range.
	/// </summary>
	float XYScale = 1.0f;
};

} // namespace vector_display
#endif