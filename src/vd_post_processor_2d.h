#ifndef VD_POST_PROCESSOR_2D_H
#define VD_POST_PROCESSOR_2D_H

#include "vd_sample.h"
#include <godot_cpp/classes/visual_instance3d.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

using namespace godot;

namespace vector_display {

/// <summary>
/// If applied to a Camera, post processing will be applied to this Camera's samples
/// just after they have been transformed and clipped to screen space.
/// If applied to a VectorDisplay, post processing will be applied to ALL Camera's
/// samples just before they are sent off to the frame buffer.
/// </summary>
class VDPostProcessor2D : public godot::Node {
	GDCLASS(VDPostProcessor2D, godot::Node)

private:
protected:
	static void _bind_methods();

public:
	VDPostProcessor2D();
	~VDPostProcessor2D();

	/// <summary>
	/// This gets the raw samples with no transforms (local or global) applied.
	/// </summary>
	virtual void process_samples_2d(TypedArray<PackedVector3Array> samples);

	GDVIRTUAL1(_process_samples_2d, TypedArray<PackedVector3Array>);
};

} // namespace vector_display

#endif