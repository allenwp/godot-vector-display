#include "vd_dot_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDDot3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sample_count", "value"), &VDDot3D::set_sample_count);
	ClassDB::bind_method(D_METHOD("get_sample_count"), &VDDot3D::get_sample_count);
	ClassDB::add_property("VDDot3D", PropertyInfo(Variant::INT, "sample_count"), "set_sample_count", "get_sample_count");
}

VDDot3D::VDDot3D() {
	sample_count = 2;
}

VDDot3D::~VDDot3D() {
}

void VDDot3D::set_sample_count(const int value) {
	sample_count = value;
}

int VDDot3D::get_sample_count() const {
	return sample_count;
}

void VDDot3D::_process(double delta) {
}

TypedArray<PackedVector4Array> VDDot3D::get_samples_3d(float fidelity) {
	TypedArray<PackedVector4Array> result;

	PackedVector4Array sample3DArray;
	sample3DArray.resize(sample_count);
	for (int i = 0; i < sample_count; i++) {
		sample3DArray[i] = VDSample3D(0.0, 0.0, 0.0, 1.0);
	}
	result.push_back(sample3DArray);

	return result;
}

void vector_display::VDDot3D::update_aabb() {
	set_aabb(AABB(Vector3(0, 0 , 0), Vector3(0, 0, 0)));
	GDVIRTUAL_CALL(_update_aabb);
}
