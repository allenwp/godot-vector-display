#ifndef VD_EDITOR_PREVIEW_3D_H
#define VD_EDITOR_PREVIEW_3D_H

#include "godot_cpp/classes/node3d.hpp"

using namespace godot;

namespace vector_display {

class VDEditorPreview3D : public Node3D {
	GDCLASS(VDEditorPreview3D, Node3D)

protected:
	static void _bind_methods();

	TypedArray<PackedVector4Array> _last_world_space_samples;

public:
	void update_preview(TypedArray<PackedVector4Array> worldSpaceSamples);
	TypedArray<PackedVector4Array> get_latest_world_space_samples();
};

} // namespace vector_display
#endif // VD_EDITOR_PREVIEW_3D_H