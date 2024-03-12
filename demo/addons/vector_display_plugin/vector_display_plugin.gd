@tool
class_name VectorDisplayPlugin extends EditorPlugin

const VDEditorPreview3DGizmo = preload("res://addons/vector_display_plugin/vd_editor_preview_3d_gizmo.gd")
var gizmo_plugin: VDEditorPreview3DGizmo = VDEditorPreview3DGizmo.new()

func _enter_tree() -> void:
	add_node_3d_gizmo_plugin(gizmo_plugin)


func _exit_tree() -> void:
	remove_node_3d_gizmo_plugin(gizmo_plugin)
