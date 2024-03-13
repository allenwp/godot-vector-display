class_name VDEditorPreview3DGizmo extends EditorNode3DGizmoPlugin

func _init() -> void:
	create_material("main", Color(1,0.5,0.5))

func _has_gizmo(node: Node3D) -> bool:
	return node is VDEditorPreview3D


func _redraw(gizmo: EditorNode3DGizmo) -> void:
	gizmo.clear()

	var preview_node := gizmo.get_node_3d() as VDEditorPreview3D
	var world_space_samples: Array[Array] = preview_node.get_latest_world_space_samples();

	VDShape3DGizmo.add_samples_as_lines(world_space_samples, gizmo, get_material("main", gizmo));
