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

	void _process(double delta) override;

	/// <summary>
	/// This gets the raw samples with no transforms (local or global) applied.
	/// </summary>
	/// <param name="fidelity"></param>
	/// <returns>TypedArray<Array> is a TypedArray<TypedArray<Vector4>> // TODO: change to TypedArray<PackedVector4Array></returns>
	virtual TypedArray<Array> get_samples_3d(float fidelity);

	TypedArray<Array> get_samples_3d_global(float fidelity);
	void apply_global_transform(TypedArray<Array> samples3D);

	
	GDVIRTUAL1R(TypedArray<Array>, _get_samples_3d, float);
};

} // namespace vector_display

#endif