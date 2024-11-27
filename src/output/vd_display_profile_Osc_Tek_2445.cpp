#include "vd_display_profile_Osc_Tek_2445.h"
#include "godot_cpp/core/math.hpp"

using namespace vector_display;

VDDisplayProfileOscTek2445::VDDisplayProfileOscTek2445() {
	AspectRatio = 10.0f / 8.0f;
	FullBrightnessOutput = -1.0f;
	ZeroBrightnessOutput = 1.0f;
	XYScale = 1.0f;
}

VDDisplayProfileOscTek2445::~VDDisplayProfileOscTek2445() {
}

VDSample vector_display::VDDisplayProfileOscTek2445::GetBlankingSample() {
	// By placing this enitrely out of the display's visible range it allows
	// the intensity to go very high without working about being able to see
	// the blanking sample.
	return VDSample(0, AspectRatio, 0);
}
