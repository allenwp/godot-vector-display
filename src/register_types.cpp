#include "register_types.h"

#include "vector_display.h"
#include "vd_shape_3d.h"
#include "shapes/3d/vd_circle_3d.h"
#include "shapes/3d/vd_dot_3d.h"
#include "shapes/3d/vd_line_3d.h"
#include "shapes/3d/vd_curly_spire_3d.h"
#include "vd_editor_preview_3d.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;
using namespace vector_display;

void initialize_vector_display_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<VectorDisplay>();
	ClassDB::register_class<VDShape3D>();
	ClassDB::register_class<VDCircle3D>();
	ClassDB::register_class<VDDot3D>();
	ClassDB::register_class<VDLine3D>();
	ClassDB::register_class<VDCurlySpire3D>();
	ClassDB::register_class<VDEditorPreview3D>();
}

void uninitialize_vector_display_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT vector_display_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_vector_display_module);
	init_obj.register_terminator(uninitialize_vector_display_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}