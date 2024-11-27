#include "vd_dac_profile_presonus_studio_26c.h"
#include "godot_cpp/core/math.hpp"

using namespace vector_display;

VDDACProfilePresonusStudio26c::VDDACProfilePresonusStudio26c() {
	samples_per_second = 192000;
	blanking_channel_delay = 18;
	FidelityScale = 1.0f;
}

VDDACProfilePresonusStudio26c::~VDDACProfilePresonusStudio26c() {
}

int VDDACProfilePresonusStudio26c::BlankingLength(VDSample sample1, VDSample sample2) {
	// blankingLength is the number of samples for each blank at a distance of 1 unit between samples
	float blankingLength = 14.0f;

	float distance = VDSampleHelper::DistanceBetweenSamples(sample1, sample2);
	if (distance < 0.01f) {
		// they're so close together, no blanking is needed.
		return 0;
	} else {
		return ceilf(blankingLength * distance);
	}
}