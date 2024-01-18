#include "vd_sample.h"

using namespace vector_display;

VDSample VDSampleHelper::GetBlankingSample() {
	return VDSample(-1.0f * FrameOutput.DisplayProfile.AspectRatio, -1.0f, 0.0f);
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