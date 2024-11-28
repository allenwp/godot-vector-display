#ifndef VD_SAMPLE_H
#define VD_SAMPLE_H

#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector4.hpp"

using namespace godot;

namespace vector_display {

/// <summary>
/// A single sample to be passed on to the vector display. This value has not yet been adjusted
/// to match the electrical signal, so it may have a range outside of -1 to 1.
/// This coordinate system is called "screen space".
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