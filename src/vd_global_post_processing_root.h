#pragma once

#include <godot_cpp/classes/node.hpp>

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
};

} // namespace vector_display