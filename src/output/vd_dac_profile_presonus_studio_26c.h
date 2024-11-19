#pragma once

#include "vd_dac_profile.h"

namespace vector_display {

/// <summary>
/// Display Profile for the Tektronix 2445 150 MHz Oscilloscope
/// </summary>
class VDDACProfilePresonusStudio26c : VDDACProfile {
public:
	VDDACProfilePresonusStudio26c();
	~VDDACProfilePresonusStudio26c();

	int BlankingLength(VDSample sample1, VDSample sample2) override;
};

} // namespace vector_display
