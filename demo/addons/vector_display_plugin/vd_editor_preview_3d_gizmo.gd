class_name VDEditorPreview3DGizmo extends EditorNode3DGizmoPlugin

func _init() -> void:
	create_material("main", Color(1,0,0))
	create_handle_material("handles")

func _has_gizmo(node: Node3D) -> bool:
	return node is VDEditorPreview3D


func _redraw(gizmo: EditorNode3DGizmo) -> void:
	gizmo.clear()

	var preview_node := gizmo.get_node_3d() as VDEditorPreview3D

	var lines := PackedVector3Array()
	var world_space_samples: Array[Array] = preview_node.get_latest_world_space_samples();
	for child_array: Array in world_space_samples:
		for i in range(child_array.size() - 1):
			var sample: Vector4 = child_array[i]
			var next_sample: Vector4 = child_array[i + 1]
			if (sample.w > 0 && next_sample.w > 0):
				lines.push_back(Vector3(sample.x, sample.y, sample.z))
				if next_sample == sample:
					# This is a dot, so make it a small line
					next_sample += Vector4(0.01, 0.01, 0.01, 0.0) # TODO: make this better
				lines.push_back(Vector3(next_sample.x, next_sample.y, next_sample.z))
	gizmo.add_lines(lines, get_material("main", gizmo), false)
