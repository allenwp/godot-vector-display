#ifndef VD_DISPLAY_PROFILE_OSC_TEK_2445_H
#define VD_DISPLAY_PROFILE_OSC_TEK_2445_H

#include "vd_display_profile.h"

namespace vector_display {

/// <summary>
/// Display Profile for the Tektronix 2445 150 MHz Oscilloscope
/// </summary>
class VDDisplayProfileOscTek2445 : VDDisplayProfile {
public:
	VDDisplayProfileOscTek2445();
	~VDDisplayProfileOscTek2445();

	virtual VDSample GetBlankingSample();
};

} // namespace vector_display

#endif