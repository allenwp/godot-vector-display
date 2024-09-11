#include "vd_post_processor_2d.h"

using namespace godot;
using namespace vector_display;

void VDPostProcessor2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("process_samples_2d", "samples"), &VDPostProcessor2D::process_samples_2d);

	GDVIRTUAL_BIND(_process_samples_2d, "samples");
}

VDPostProcessor2D::VDPostProcessor2D() {}
VDPostProcessor2D::~VDPostProcessor2D() {}

void VDPostProcessor2D::process_samples_2d(TypedArray<PackedVector3Array> samples) {
	TypedArray<PackedVector3Array> ret;
	GDVIRTUAL_CALL(_process_samples_2d, samples);
}
