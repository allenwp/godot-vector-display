#include "vd_global_post_processing_root.h"

using namespace godot;
using namespace vector_display;

void VDGlobalPostProcessingRoot::_bind_methods() {
}

void vector_display::VDGlobalPostProcessingRoot::_enter_tree() {
	Node::_enter_tree();
	add_to_group(StringName(VD_GLOBAL_POST_PROCESSING_ROOT_GROUP_NAME));
}
