#ifndef VD_POST_PROCESSOR_3D_H
#define VD_POST_PROCESSOR_3D_H

#include "vd_sample.h"
#include <godot_cpp/classes/visual_instance3d.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>

using namespace godot;

namespace vector_display {

/// <summary>
/// If applied as a child of a VDShape3D, this post-processing will occur in LOCAL space
/// before the Shape's WorldTransform is applied.
/// If applied as child of a Camera, this post-processing will occur in WORLD space
/// to an entire Camera's Sample3D stream before it has been transformed to screen space.
/// </summary>
class VDPostProcessor3D : public godot::Node {
	GDCLASS(VDPostProcessor3D, godot::Node)

private:
protected:
	static void _bind_methods();

public:
	VDPostProcessor3D();
	~VDPostProcessor3D();

	/// <summary>
	/// This gets the raw samples with no transforms (local or global) applied.
	/// </summary>
	virtual void process_samples_3d(TypedArray<PackedVector4Array> samples3D);

	GDVIRTUAL1(_process_samples_3d, TypedArray<PackedVector4Array>);
};

} // namespace vector_display

#endif