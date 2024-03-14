@tool
class_name GizmoUpdater extends Node3D
var gizmo: EditorNode3DGizmo;
var gizmo_plugin: EditorNode3DGizmoPlugin;

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	gizmo_plugin._redraw(gizmo)
