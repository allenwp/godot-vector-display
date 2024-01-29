#ifndef VD_SHAPE_3D_H
#define VD_SHAPE_3D_H

#include <godot_cpp/classes/node.hpp>

namespace vector_display {

class VDShape3D : public godot::Node {
	GDCLASS(VDShape3D, godot::Node)

private:

protected:
	static void _bind_methods();

public:
	VDShape3D();
	~VDShape3D();

	void _process(double delta) override;
};

} // namespace vector_display

#endif