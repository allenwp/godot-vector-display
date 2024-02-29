#include "vd_circle_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDCircle3D::_bind_methods() {
}

VDCircle3D::VDCircle3D() {
	baseSampleCount = 1000;
}

VDCircle3D::~VDCircle3D() {
}

void VDCircle3D::_process(double delta) {
}

TypedArray<Array> VDCircle3D::get_samples_3d(float fidelity) {
	TypedArray<Array> result;

	int sampleCount = (int)round(baseSampleCount * fidelity);

	TypedArray<Vector4> sample3DArray; // TODO: Change to TypedArray<PackedVector4Array>
	sample3DArray.resize(sampleCount);
	for (int i = 0; i < sampleCount; i++) {
		auto value = Math::lerp(0, (float)(Math_PI * 2), (float)i / (float)(sampleCount - 1));
		VDSample3D sample = VDSample3D(sin(value), cos(value), 0, 1.0f);
		sample3DArray[i] = sample;
	}
	result.push_back(sample3DArray);

	return result;
}
