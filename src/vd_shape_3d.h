#ifndef VD_SHAPE_3D_H
#define VD_SHAPE_3D_H

#include <godot_cpp/classes/visual_instance3d.hpp>
#include "vd_sample.h"
#include <godot_cpp/core/gdvirtual.gen.inc>

using namespace godot;

namespace vector_display {

class VDShape3D : public godot::VisualInstance3D {
	GDCLASS(VDShape3D, godot::VisualInstance3D)

private:

protected:
	static void _bind_methods();

public:
	VDShape3D();
	~VDShape3D();

	/// <summary>
	/// This gets the raw samples with no transforms (local or global) applied.
	/// </summary>
	virtual TypedArray<PackedVector4Array> get_samples_3d(float fidelity);

	TypedArray<PackedVector4Array> get_samples_3d_global(float fidelity);
	void apply_global_transform(TypedArray<PackedVector4Array> samples3D);

	
	GDVIRTUAL1R(TypedArray<PackedVector4Array>, _get_samples_3d, float);
};

} // namespace vector_display

#endif