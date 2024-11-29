#include "vd_camera_3d.h"

using namespace godot;
using namespace vector_display;

void VDCamera3D::_bind_methods() {
}

void vector_display::VDCamera3D::_enter_tree() {
	Camera3D::_enter_tree();
	add_to_group(StringName(VD_CAMERA_3D_GROUP_NAME));
}
