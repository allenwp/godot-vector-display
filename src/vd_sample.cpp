#include "vd_sample.h"

using namespace vector_display;

VDSample VDSampleHelper::Blank() {
	VDSample blank;
	// TODO: blank.x = -1.0f * FrameOutput.DisplayProfile.AspectRatio;
	blank.y = -1.0f;
	blank.z = 0.0f;
	return blank;
}

/// <summary>
/// Clamps the sample to (-1, -1) and (1, 1)
/// </summary>
VDSample VDSampleHelper::Clamp(VDSample sample) {
	if (sample.x < -1.0f) {
		sample.x = -1.0f;
	} else if (sample.x > 1.0f) {
		sample.x = 1.0f;
	}

	if (sample.y < -1.0f) {
		sample.y = -1.0f;
	} else if (sample.y > 1.0f) {
		sample.y = 1.0f;
	}
	
	return sample;
}