#include "vd_sample.h"
#include "output/vd_frame_output.h"
#include "godot_cpp/variant/vector2.hpp"

using namespace vector_display;
using namespace godot;

float VDSampleHelper::DistanceBetweenSamples(VDSample sample1, VDSample sample2) {
	return Vector2(sample1.x, sample1.y).distance_to(Vector2(sample2.x, sample2.y));
}