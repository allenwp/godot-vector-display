#include "vd_circle_3d.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VDCircle3D::_bind_methods() {
}

VDCircle3D::VDCircle3D() {
	
}

VDCircle3D::~VDCircle3D() {
}

void VDCircle3D::_process(double delta) {
	UtilityFunctions::print("Circle");
}