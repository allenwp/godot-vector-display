#include "vd_editor_preview_3d.h"

using namespace godot;
using namespace vector_display;

void VDEditorPreview3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_latest_world_space_samples"), &VDEditorPreview3D::get_latest_world_space_samples);
}

void VDEditorPreview3D::update_preview(TypedArray<Array> worldSpaceSamples) {
	_last_world_space_samples = worldSpaceSamples;
	// TODO: somehow notifiy the editor that this object's gizmo needs redrawing
}

TypedArray<Array> VDEditorPreview3D::get_latest_world_space_samples() {
	return _last_world_space_samples;
}