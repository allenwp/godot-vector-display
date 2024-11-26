#include "vd_sample.h"
#include "output/vd_frame_output.h"
#include "godot_cpp/variant/vector2.hpp"

using namespace vector_display;
using namespace godot;

VDSample VDSampleHelper::GetBlankingSample() {
	return VDSample(0, 0, 0);
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

float VDSampleHelper::DistanceBetweenSamples(VDSample sample1, VDSample sample2) {
	return Vector2(sample1.x, sample1.y).distance_to(Vector2(sample2.x, sample2.y));
}