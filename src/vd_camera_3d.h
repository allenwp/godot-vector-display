#pragma once

#include <godot_cpp/classes/camera3d.hpp>

#define VD_CAMERA_3D_GROUP_NAME "vd_camera_3d"

using namespace godot;

namespace vector_display {

class VDCamera3D : public godot::Camera3D {
	GDCLASS(VDCamera3D, godot::Camera3D)

private:
protected:
	static void _bind_methods();

public:
	VDCamera3D();
	virtual void _enter_tree() override;
};

} // namespace vector_display
