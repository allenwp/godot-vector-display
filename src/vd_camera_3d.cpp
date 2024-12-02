#include "vd_camera_3d.h"

using namespace godot;
using namespace vector_display;

void VDCamera3D::_bind_methods() {
}

vector_display::VDCamera3D::VDCamera3D() :
		Camera3D() {
	// VDCamera3D often uses a different viewport than the raster camera(s) to ensure its viewport aspect ratio matches
	// that of the vector display device. If VDCamera3D did not cull the layer of raster nodes (1), it would re-draw the raster
	// scene unnecessarily. By defaulting VDShape3D nodes to layer 11 and masking layer 1-10 on VDCamera3D, it becomes
	// easier to ensure that raster nodes are not re-rendered by the VDCamera3D.
	for (int i = 1; i < 11; i++) {
		set_cull_mask_value(i, false);
	}
}

void vector_display::VDCamera3D::_enter_tree() {
	Camera3D::_enter_tree();
	add_to_group(StringName(VD_CAMERA_3D_GROUP_NAME));
}
