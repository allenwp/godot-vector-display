@tool
class_name VectorDisplayPlugin extends EditorPlugin

const VdShape3dGizmo = preload("res://addons/vector_display_plugin/vd_shape_3d_gizmo.gd")
var shape_3d_gizmo_plugin: VDShape3DGizmo = VDShape3DGizmo.new()

const VDEditorPreview3DGizmo = preload("res://addons/vector_display_plugin/vd_editor_preview_3d_gizmo.gd")
var editor_preview_3d_gizmo_plugin: VDEditorPreview3DGizmo = VDEditorPreview3DGizmo.new()

func _enter_tree() -> void:
	#add_custom_type("VDLine3D", "VDShape3D", preload("vd_line_3d.gd"), preload("vd_shape.png"))
	add_node_3d_gizmo_plugin(shape_3d_gizmo_plugin)
	add_node_3d_gizmo_plugin(editor_preview_3d_gizmo_plugin)


func _exit_tree() -> void:
	remove_node_3d_gizmo_plugin(shape_3d_gizmo_plugin)
	remove_node_3d_gizmo_plugin(editor_preview_3d_gizmo_plugin)
	#remove_custom_type("VDLine3D")
