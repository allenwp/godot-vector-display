#include "vd_dot_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDDot3D::_bind_methods() {
}

VDDot3D::VDDot3D() {
}

VDDot3D::~VDDot3D() {
}

void VDDot3D::_process(double delta) {
}

TypedArray<Array> VDDot3D::get_samples_3d(float fidelity) {
	TypedArray<Array> result; // TODO: Change to TypedArray<PackedVector4Array>

	TypedArray<Vector4> sample3DArray;
	sample3DArray.resize(SampleCount);
	for (int i = 0; i < SampleCount; i++) {
		sample3DArray[i] = VDSample3D(0.0, 0.0, 0.0, 1.0);
	}
	result.push_back(sample3DArray);

	return result;
}
