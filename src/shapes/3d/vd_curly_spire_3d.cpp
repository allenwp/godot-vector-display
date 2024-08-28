#include "vd_curly_spire_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDCurlySpire3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_base_sample_count", "value"), &VDCurlySpire3D::set_base_sample_count);
	ClassDB::bind_method(D_METHOD("get_base_sample_count"), &VDCurlySpire3D::get_base_sample_count);
	ClassDB::add_property("VDCurlySpire3D", PropertyInfo(Variant::INT, "base_sample_count"), "set_base_sample_count", "get_base_sample_count");

	ClassDB::bind_method(D_METHOD("set_curl_count", "value"), &VDCurlySpire3D::set_curl_count);
	ClassDB::bind_method(D_METHOD("get_curl_count"), &VDCurlySpire3D::get_curl_count);
	ClassDB::add_property("VDCurlySpire3D", PropertyInfo(Variant::INT, "curl_count"), "set_curl_count", "get_curl_count");
}

VDCurlySpire3D::VDCurlySpire3D() {
	base_sample_count = 300;
	curl_count = 5;
}

VDCurlySpire3D::~VDCurlySpire3D() {
}

void VDCurlySpire3D::set_base_sample_count(const int value) {
	base_sample_count = value;
}

int VDCurlySpire3D::get_base_sample_count() const {
	return base_sample_count;
}

void VDCurlySpire3D::set_curl_count(const int value) {
	curl_count = value;
}

int VDCurlySpire3D::get_curl_count() const {
	return curl_count;
}

TypedArray<PackedVector4Array> VDCurlySpire3D::get_samples_3d(float fidelity) {
	const float height = 1.0f;

	TypedArray<PackedVector4Array> result;

	int sampleCount = (int)round(base_sample_count * fidelity);

	PackedVector4Array sample3DArray;
	sample3DArray.resize(sampleCount);
	for (int i = 0; i < sampleCount; i++) {
		float value = (float)i / (sampleCount - 1);
		float valueRadians = (float)(value * Math_PI * 2 * curl_count);
		float widthValue = value * 2.0f;
		if (value > 0.5f) {
			widthValue = (widthValue - 2.0f) * -1.0f;
		}
		VDSample3D sample;
		sample.y = value * height;
		sample.x = (float)sin(valueRadians) * widthValue;
		sample.z = (float)cos(valueRadians) * widthValue;
		VD_SAMPLE_3D_BRIGHTNESS(sample) = 1.0f;
		sample3DArray[i] = sample;

	}
	result.push_back(sample3DArray);

	return result;
}
