@tool
extends EditorPlugin

func _enter_tree() -> void:
	add_custom_type("VDScriptLine3D", "VDShape3D", preload("vd_line_3d.gd"), preload("vd_shape.png"))
	add_custom_type("VDCurlyCircle", "VDShape3D", preload("vd_curly_circle.gd"), preload("vd_shape.png"))

	add_custom_type("VDStrobePostProcessor", "VDPostProcessor3D", preload("vd_strobe_post_processor.gd"), preload("vd_shape.png"))
	add_custom_type("Scramble3dPostProcessor", "VDPostProcessor3D", preload("vd_scramble_3d_post_processor.gd"), preload("vd_shape.png"))

	add_custom_type("Scramble2dPostProcessor", "VDPostProcessor2D", preload("vd_scramble_2d_post_processor.gd"), preload("vd_shape.png"))

func _exit_tree() -> void:
	remove_custom_type("VDScriptLine3D")
	remove_custom_type("VDCurlyCircle")

	remove_custom_type("VDStrobePostProcessor")
	remove_custom_type("Scramble3dPostProcessor")

	remove_custom_type("Scramble2dPostProcessor")
