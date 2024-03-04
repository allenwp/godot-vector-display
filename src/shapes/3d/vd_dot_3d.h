#ifndef VD_DOT_3D_H
#define VD_DOT_3D_H

#include "vd_shape_3d.h"

namespace vector_display {

/// <summary>
/// A fidelity-independent dot of a specified sample count.
/// </summary>
class VDDot3D : public VDShape3D {
	GDCLASS(VDDot3D, VDShape3D)

private:

protected:
	static void _bind_methods();

public:
	VDDot3D();
	~VDDot3D();

	void _process(double delta) override;
	int SampleCount = 2; // TODO: Expose this
	virtual TypedArray<Array> get_samples_3d(float fidelity) override;
};

} // namespace vector_display

#endif