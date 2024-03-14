@tool
extends Node3D
var gizmo: EditorNode3DGizmo;
var gizmo_plugin: EditorNode3DGizmoPlugin;

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if gizmo_plugin != null \
		&& !gizmo_plugin.is_queued_for_deletion() \
		&& !gizmo.is_queued_for_deletion() \
		&& gizmo.get_node_3d() != null \
		&& !gizmo.get_node_3d().is_queued_for_deletion():
			gizmo_plugin._redraw(gizmo)
