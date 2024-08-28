#ifndef VD_HELIX_CIRCLE_3D_H
#define VD_HELIX_CIRCLE_3D_H

#include "vd_shape_3d.h"

namespace vector_display {

class VDHelixCircle3D : public VDShape3D {
	GDCLASS(VDHelixCircle3D, VDShape3D)

private:

protected:
	static void _bind_methods();
	int base_sample_count;
	int curl_count;
	float thickness;

public:
	VDHelixCircle3D();
	~VDHelixCircle3D();

	void set_base_sample_count(const int value);
	int get_base_sample_count() const;

	void set_curl_count(const int value);
	int get_curl_count() const;

	void set_thickness(const float value);
	float get_thickness() const;

	virtual TypedArray<PackedVector4Array> get_samples_3d(float fidelity) override;

	PackedVector4Array get_coil(int sample_count, bool inverse);
};

} // namespace vector_display

#endif