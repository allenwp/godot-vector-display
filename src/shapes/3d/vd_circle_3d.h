#ifndef VD_CIRCLE_3D_H
#define VD_CIRCLE_3D_H

#include "vd_shape_3d.h"

namespace vector_display {

class VDCircle3D : public VDShape3D {
	GDCLASS(VDCircle3D, VDShape3D)

private:

protected:
	static void _bind_methods();

public:
	VDCircle3D();
	~VDCircle3D();

	void _process(double delta) override;
};

} // namespace vector_display

#endif