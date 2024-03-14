#include "vd_shape_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDShape3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_samples_3d", "fidelity"), &VDShape3D::get_samples_3d);
	ClassDB::bind_method(D_METHOD("get_samples_3d_global", "fidelity"), &VDShape3D::get_samples_3d_global);
	ClassDB::bind_method(D_METHOD("apply_global_transform", "samples3D"), &VDShape3D::apply_global_transform);

	GDVIRTUAL_BIND(_get_samples_3d, "fidelity");
}

VDShape3D::VDShape3D() {
	
}

VDShape3D::~VDShape3D() {
}

void VDShape3D::_process(double delta) {
}

TypedArray<Array> VDShape3D::get_samples_3d(float fidelity) {
	TypedArray<Array> ret;
	if (GDVIRTUAL_CALL(_get_samples_3d, fidelity, ret)) {
		return ret;
	}
	return ret;
}

TypedArray<Array> VDShape3D::get_samples_3d_global(float fidelity) {
	TypedArray<Array> samples3D = get_samples_3d(fidelity);
	apply_global_transform(samples3D);
	return samples3D;
}

void VDShape3D::apply_global_transform(TypedArray<Array> samples3D) {
	Transform3D worldTransform = get_global_transform();
	for (int j = 0; j < samples3D.size(); j++) {
		Array samples3DArray = samples3D[j]; // must be contained in an Array instead of a TypedArray because of https://github.com/godotengine/godot/issues/89191
		for (int i = 0; i < samples3DArray.size(); i++) {
			Variant element = samples3DArray[i];
			ERR_CONTINUE_EDMSG(element.get_type() != Variant::VECTOR4, "Element of sample array is not a Vector4.");
			VDSample3D sample = element;
			Vector3 position = worldTransform.xform(Vector3(sample.x, sample.y, sample.z));
			samples3DArray[i] = VDSample3D(position.x, position.y, position.z, sample.w);
		}
	}
}
