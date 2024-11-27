#ifndef VD_CURLY_SPIRE_3D_H
#define VD_CURLY_SPIRE_3D_H

#include "vd_shape_3d.h"

namespace vector_display {

class VDCurlySpire3D : public VDShape3D {
	GDCLASS(VDCurlySpire3D, VDShape3D)

private:

protected:
	static void _bind_methods();
	int base_sample_count;
	int curl_count;

public:
	VDCurlySpire3D();
	~VDCurlySpire3D();

	void set_base_sample_count(const int value);
	int get_base_sample_count() const;

	void set_curl_count(const int value);
	int get_curl_count() const;

	virtual TypedArray<PackedVector4Array> get_samples_3d(float fidelity) override;
	virtual void update_aabb() override;
};

} // namespace vector_display

#endif