#include "vd_post_processor_3d.h"

using namespace godot;
using namespace vector_display;

void VDPostProcessor3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("process_samples_3d", "samples3D"), &VDPostProcessor3D::process_samples_3d);

	GDVIRTUAL_BIND(_process_samples_3d, "samples_3d");
}

VDPostProcessor3D::VDPostProcessor3D() {}
VDPostProcessor3D::~VDPostProcessor3D() {}

void VDPostProcessor3D::process_samples_3d(TypedArray<PackedVector4Array> samples3D) {
	TypedArray<PackedVector4Array> ret;
	GDVIRTUAL_CALL(_process_samples_3d, samples3D);
}
