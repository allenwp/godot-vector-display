#include "vd_circle_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDCircle3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_base_sample_count", "value"), &VDCircle3D::set_base_sample_count);
	ClassDB::bind_method(D_METHOD("get_base_sample_count"), &VDCircle3D::get_base_sample_count);
	ClassDB::add_property("VDCircle3D", PropertyInfo(Variant::INT, "base_sample_count"), "set_base_sample_count", "get_base_sample_count");
}

VDCircle3D::VDCircle3D() {
	base_sample_count = 1000;
}

VDCircle3D::~VDCircle3D() {
}

void VDCircle3D::set_base_sample_count(const int value) {
	base_sample_count = value;
}

int VDCircle3D::get_base_sample_count() const {
	return base_sample_count;
}

void VDCircle3D::_process(double delta) {
}

TypedArray<PackedVector4Array> VDCircle3D::get_samples_3d(float fidelity) {
	TypedArray<PackedVector4Array> result;

	int sampleCount = (int)round(base_sample_count * fidelity);

	PackedVector4Array sample3DArray;
	sample3DArray.resize(sampleCount);
	for (int i = 0; i < sampleCount; i++) {
		auto value = Math::lerp(0, (float)(Math_PI * 2), (float)i / (float)(sampleCount - 1));
		VDSample3D sample = VDSample3D(sin(value), cos(value), 0, 1.0f);
		sample3DArray[i] = sample;
	}
	result.push_back(sample3DArray);

	return result;
}

void vector_display::VDCircle3D::update_aabb() {
	set_aabb(AABB(Vector3(-1, -1, 0), Vector3(2, 2, 0)));
	GDVIRTUAL_CALL(_update_aabb);
}
