#include "vd_display_profile_Osc_Tek_TAS_465.h"
#include "godot_cpp/core/math.hpp"

using namespace vector_display;

VDDisplayProfileOscTekTAS465::VDDisplayProfileOscTekTAS465() {
	AspectRatio = 1.187f; // TODO
	FullBrightnessOutput = 0.832f;
	ZeroBrightnessOutput = 1.0f;
	FidelityScale = 1.0f;
}

VDDisplayProfileOscTekTAS465::~VDDisplayProfileOscTekTAS465() {
}

int VDDisplayProfileOscTekTAS465::BlankingLength(VDSample sample1, VDSample sample2) {
	// blankingLength is the number of samples for each blank at a distance of 1 unit between samples
	float blankingLength = 14.0f; // TODO

	// Clamp these because that's what's going to happen at output time anyway
	sample1 = VDSampleHelper::Clamp(sample1);
	sample2 = VDSampleHelper::Clamp(sample2);
	float distance = VDSampleHelper::DistanceBetweenSamples(sample1, sample2);
	if (distance < 0.01f) {
		// they're so close together, no blanking is needed.
		return 0;
	} else {
		return ceilf(blankingLength * distance);
	}
}