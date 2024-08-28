#ifndef VD_CIRCLE_3D_H
#define VD_CIRCLE_3D_H

#include "vd_shape_3d.h"

namespace vector_display {

class VDCircle3D : public VDShape3D {
	GDCLASS(VDCircle3D, VDShape3D)

private:

protected:
	static void _bind_methods();
	int base_sample_count;

public:
	VDCircle3D();
	~VDCircle3D();

	void set_base_sample_count(const int value);
	int get_base_sample_count() const;

	void _process(double delta) override;
	virtual TypedArray<PackedVector4Array> get_samples_3d(float fidelity) override;
};

} // namespace vector_display

#endif