#ifndef VD_SHAPE_3D_H
#define VD_SHAPE_3D_H

#include <godot_cpp/classes/visible_on_screen_notifier3d.hpp>
#include "vd_sample.h"
#include <godot_cpp/core/gdvirtual.gen.inc>

#define VD_SHAPE_3D_GROUP_NAME "vd_shape_3d"

using namespace godot;

namespace vector_display {

class VDShape3D : public godot::VisibleOnScreenNotifier3D {
	GDCLASS(VDShape3D, godot::VisibleOnScreenNotifier3D)

private:

protected:
	static void _bind_methods();

public:
	VDShape3D();
	virtual void _ready() override;
	virtual void _enter_tree() override;

	/// <summary>
	/// This gets the raw samples with no transforms (local or global) applied.
	/// </summary>
	virtual TypedArray<PackedVector4Array> get_samples_3d(float fidelity);
	/// <summary>
	/// Override to update VisibleOnScreenNotifier3D's AABB before get_samples_3d is called.
	/// </summary>
	virtual void update_aabb();

	TypedArray<PackedVector4Array> get_samples_3d_global(float fidelity);
	void apply_global_transform(TypedArray<PackedVector4Array> samples3D);

	GDVIRTUAL1R(TypedArray<PackedVector4Array>, _get_samples_3d, float);
	GDVIRTUAL0(_update_aabb);
};

} // namespace vector_display

#endif