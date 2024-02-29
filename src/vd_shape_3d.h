#ifndef VD_SHAPE_3D_H
#define VD_SHAPE_3D_H

#include <godot_cpp/classes/visual_instance3d.hpp>
#include "vd_sample.h"

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

	virtual TypedArray<Array> get_samples_3d(float fidelity);
};

} // namespace vector_display

#endif