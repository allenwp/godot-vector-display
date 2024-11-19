#pragma once

#include "vd_sample.h"

namespace vector_display {

class VDDACProfile {
public:
	int samples_per_second = 192000;
	int blanking_channel_delay = 0;

	// TODO: maybe change the blanking length thing into something that actually returns
	// the blanking array between two samples? This way the whole blanking algorithm
	// could be DAC specific.
	virtual int BlankingLength(VDSample sample1, VDSample sample2) = 0;
	
	float FidelityScale = 1.0f;
};

} // namespace vector_display