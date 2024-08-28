#include "vd_line_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDLine3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_base_sample_count", "value"), &VDLine3D::set_base_sample_count);
	ClassDB::bind_method(D_METHOD("get_base_sample_count"), &VDLine3D::get_base_sample_count);
	ClassDB::add_property("VDLine3D", PropertyInfo(Variant::INT, "base_sample_count"), "set_base_sample_count", "get_base_sample_count");

	ClassDB::bind_method(D_METHOD("set_start", "value"), &VDLine3D::set_start);
	ClassDB::bind_method(D_METHOD("get_start"), &VDLine3D::get_start);
	ClassDB::add_property("VDLine3D", PropertyInfo(Variant::VECTOR3, "start"), "set_start", "get_start");

	ClassDB::bind_method(D_METHOD("set_end", "value"), &VDLine3D::set_end);
	ClassDB::bind_method(D_METHOD("get_end"), &VDLine3D::get_end);
	ClassDB::add_property("VDLine3D", PropertyInfo(Variant::VECTOR3, "end"), "set_end", "get_end");
}

VDLine3D::VDLine3D() {
	base_sample_count = 50;
}

VDLine3D::~VDLine3D() {
}

void VDLine3D::set_base_sample_count(const int value) {
	base_sample_count = value;
}

int VDLine3D::get_base_sample_count() const {
	return base_sample_count;
}

void VDLine3D::set_start(const Vector3 value) {
	start = value;
}

Vector3 VDLine3D::get_start() const {
	return start;
}

void VDLine3D::set_end(const Vector3 value) {
	end = value;
}

Vector3 VDLine3D::get_end() const {
	return end;
}

void VDLine3D::_process(double delta) {
}

TypedArray<PackedVector4Array> VDLine3D::get_samples_3d(float fidelity) {
	TypedArray<PackedVector4Array> result;
	 
	int sampleCount = (int)round(base_sample_count * fidelity);

	PackedVector4Array sample3DArray;
	sample3DArray.resize(sampleCount);
	for (int i = 0; i < sampleCount; i++) {
		Vector3 point3D = start.lerp(end, (float)i / (float)(sampleCount - 1));
		VDSample3D sample = VDSample3D(point3D.x, point3D.y, point3D.z, 1.0f);
		sample3DArray[i] = sample;
	}
	result.push_back(sample3DArray);

	return result;
}
