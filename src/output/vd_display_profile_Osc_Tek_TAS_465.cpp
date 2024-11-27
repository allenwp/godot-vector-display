#include "vd_display_profile_Osc_Tek_TAS_465.h"
#include "godot_cpp/core/math.hpp"

using namespace vector_display;

VDDisplayProfileOscTekTAS465::VDDisplayProfileOscTekTAS465() {
	AspectRatio = 10.0f / 8.0f;
	FullBrightnessOutput = 0.832f;
	ZeroBrightnessOutput = 1.0f;
	XYScale = 0.72f;
}

VDDisplayProfileOscTekTAS465::~VDDisplayProfileOscTekTAS465() {
}

VDSample vector_display::VDDisplayProfileOscTekTAS465::GetBlankingSample() {
	// By placing this enitrely out of the display's visible range it allows
	// the intensity to go very high without working about being able to see
	// the blanking sample.
	return VDSample(0, AspectRatio, 0);
}
