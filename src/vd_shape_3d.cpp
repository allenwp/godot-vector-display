#include "vd_shape_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDShape3D::_bind_methods() {
}

VDShape3D::VDShape3D() {
	
}

VDShape3D::~VDShape3D() {
}

void VDShape3D::_process(double delta) {
	UtilityFunctions::print("Shape");
}