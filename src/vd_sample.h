#ifndef VD_SAMPLE_H
#define VD_SAMPLE_H

#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector4.hpp"

using namespace godot;

namespace vector_display {

/// <summary>
/// A single sample, in screen space, to be passed on to the vector display.
/// 
/// x:	0 is the center of the screen. This should have the range of -AspectRatio to AspectRatio
///		to be visible on the screen.
///		A square aspect ratio will have the rage of -1 to 1 be visible.
/// 
/// y:	0 is the center of the screen. This should be in range of -1 to 1 to be visible, regardless
///		of aspect ratio.
/// 
/// z:	Range of 0 to 1 where 0 is no brightness, 1 is full brightness.
/// </summary>
typedef Vector3 VDSample;
#define VD_SAMPLE_BRIGHTNESS(sample) ((sample).z)

class VDSampleHelper {
public:
	static VDSample GetBlankingSample();
	
	/// <summary>
	/// Clamps the sample to (-1, -1) and (1, 1)
	/// </summary>
	static VDSample Clamp(VDSample sample);

	static float DistanceBetweenSamples(VDSample sample1, VDSample sample2);
};

/// <summary>
/// 2D sample
/// </summary>
typedef Vector3 VDSample2D;
#define VD_SAMPLE_BRIGHTNESS(sample) ((sample).z)

/// <summary>
/// 3D sample
/// </summary>
typedef Vector4 VDSample3D;
#define VD_SAMPLE_3D_BRIGHTNESS(sample) ((sample).w)

} // namespace vector_display
#endif // VD_SAMPLE_H