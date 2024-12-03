@tool
class_name VectorDisplayPlugin extends EditorPlugin

const VdShape3dGizmo = preload("res://addons/vector_display_plugin/vd_shape_3d_gizmo.gd")
var shape_3d_gizmo_plugin: VDShape3DGizmo = null

const VDEditorPreview3DGizmo = preload("res://addons/vector_display_plugin/vd_editor_preview_3d_gizmo.gd")
var editor_preview_3d_gizmo_plugin: VDEditorPreview3DGizmo = null

func _enter_tree() -> void:
	if Engine.is_editor_hint():
		shape_3d_gizmo_plugin = VDShape3DGizmo.new()
		add_node_3d_gizmo_plugin(shape_3d_gizmo_plugin)
		editor_preview_3d_gizmo_plugin = VDEditorPreview3DGizmo.new()
		add_node_3d_gizmo_plugin(editor_preview_3d_gizmo_plugin)


func _exit_tree() -> void:
	if Engine.is_editor_hint():
		remove_node_3d_gizmo_plugin(editor_preview_3d_gizmo_plugin)
		editor_preview_3d_gizmo_plugin = null
		remove_node_3d_gizmo_plugin(shape_3d_gizmo_plugin)
		shape_3d_gizmo_plugin = null
