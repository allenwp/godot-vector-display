#include "vd_helix_circle_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDHelixCircle3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_base_sample_count", "value"), &VDHelixCircle3D::set_base_sample_count);
	ClassDB::bind_method(D_METHOD("get_base_sample_count"), &VDHelixCircle3D::get_base_sample_count);
	ClassDB::add_property("VDHelixCircle3D", PropertyInfo(Variant::INT, "base_sample_count"), "set_base_sample_count", "get_base_sample_count");

	ClassDB::bind_method(D_METHOD("set_curl_count", "value"), &VDHelixCircle3D::set_curl_count);
	ClassDB::bind_method(D_METHOD("get_curl_count"), &VDHelixCircle3D::get_curl_count);
	ClassDB::add_property("VDHelixCircle3D", PropertyInfo(Variant::INT, "curl_count"), "set_curl_count", "get_curl_count");

	ClassDB::bind_method(D_METHOD("set_thickness", "value"), &VDHelixCircle3D::set_thickness);
	ClassDB::bind_method(D_METHOD("get_thickness"), &VDHelixCircle3D::get_thickness);
	ClassDB::add_property("VDHelixCircle3D", PropertyInfo(Variant::FLOAT, "thickness"), "set_thickness", "get_thickness");
}

VDHelixCircle3D::VDHelixCircle3D() {
	base_sample_count = 300;
	curl_count = 5;
	thickness = 0.1;
}

VDHelixCircle3D::~VDHelixCircle3D() {
}

void VDHelixCircle3D::set_base_sample_count(const int value) {
	base_sample_count = value;
}

int VDHelixCircle3D::get_base_sample_count() const {
	return base_sample_count;
}

void VDHelixCircle3D::set_curl_count(const int value) {
	curl_count = value;
}

int VDHelixCircle3D::get_curl_count() const {
	return curl_count;
}

void VDHelixCircle3D::set_thickness(const float value) {
	thickness = value;
}

float VDHelixCircle3D::get_thickness() const {
	return thickness;
}

TypedArray<PackedVector4Array> VDHelixCircle3D::get_samples_3d(float fidelity) {
	TypedArray<PackedVector4Array> result;

	int sampleCount = (int)round(base_sample_count * fidelity);
	result.push_back(get_coil(sampleCount, false));
	result.push_back(get_coil(sampleCount, true));

	return result;
}

void vector_display::VDHelixCircle3D::update_aabb() {
	set_aabb(AABB(Vector3(-1.0 - thickness, -1.0 - thickness, -1.0 * thickness), Vector3((1.0 + thickness) * 2, (1.0 + thickness) * 2, thickness * 2.0)));
	GDVIRTUAL_CALL(_update_aabb);
}

PackedVector4Array VDHelixCircle3D::get_coil(int sample_count, bool inverse) {
	PackedVector4Array sample3DArray;
	sample3DArray.resize(sample_count);
	for (int i = 0; i < sample_count; i++) {
		auto value = Math::lerp(0, (float)(Math_PI * 2), (float)i / (float)(sample_count - 1));
		float extend_amount = cos(value * curl_count) * thickness * (inverse ? -1.0 : 1.0);
		Vector2 circle = Vector2(sin(value), cos(value));
		Vector2 extend = circle.normalized() * extend_amount;
		VDSample3D sample = VDSample3D(circle.x + extend.x, circle.y + extend.y, 0, 1.0f);
		sample.z = sin(value * curl_count) * thickness * (inverse ? -1.0 : 1.0);
		sample3DArray[i] = sample;
	}
	return sample3DArray;
}