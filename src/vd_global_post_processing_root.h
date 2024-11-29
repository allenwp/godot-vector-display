#pragma once

#include <godot_cpp/classes/node.hpp>

#define VD_GLOBAL_POST_PROCESSING_ROOT_GROUP_NAME "vd_global_post_processing_root"

using namespace godot;

namespace vector_display {

/// <summary>
/// If applied to a Camera, post processing will be applied to this Camera's samples
/// just after they have been transformed and clipped to screen space.
/// If applied to a VectorDisplay, post processing will be applied to ALL Camera's
/// samples just before they are sent off to the frame buffer.
/// </summary>
class VDGlobalPostProcessingRoot : public godot::Node {
	GDCLASS(VDGlobalPostProcessingRoot, godot::Node)

private:
protected:
	static void _bind_methods();

public:
	virtual void _enter_tree() override;
};

} // namespace vector_display