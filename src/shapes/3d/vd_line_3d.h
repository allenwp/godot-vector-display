#ifndef VD_LINE_3D_H
#define VD_LINE_3D_H

#include "vd_shape_3d.h"

namespace vector_display {

class VDLine3D : public VDShape3D {
	GDCLASS(VDLine3D, VDShape3D)

private:

protected:
	static void _bind_methods();
	int base_sample_count;
	Vector3 start;
	Vector3 end;

public:
	VDLine3D();
	~VDLine3D();

	void set_base_sample_count(const int value);
	int get_base_sample_count() const;

	void set_start(const Vector3 value);
	Vector3 get_start() const;

	void set_end(const Vector3 value);
	Vector3 get_end() const;

	void _process(double delta) override;
	virtual TypedArray<PackedVector4Array> get_samples_3d(float fidelity) override;
	virtual void update_aabb() override;
};

} // namespace vector_display

#endif