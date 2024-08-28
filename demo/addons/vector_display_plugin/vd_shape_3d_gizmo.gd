class_name VDShape3DGizmo extends EditorNode3DGizmoPlugin

func _init() -> void:
	create_material("main", Color(0.5,1,0.5))

func _get_gizmo_name() -> String:
	return "VDShape3D Gizmo"

func _has_gizmo(node: Node3D) -> bool:
	GizmoUpdater.gizmo = null
	GizmoUpdater.gizmo_plugin = null
	return node is VDShape3D

func _redraw(gizmo: EditorNode3DGizmo) -> void:
	gizmo.clear()
	# disable this and reload project to prevent crash:
	GizmoUpdater.gizmo = gizmo
	GizmoUpdater.gizmo_plugin = self

	var node3d: Node3D = gizmo.get_node_3d()
	var shape := node3d as VDShape3D
	var world_space_samples: Array[PackedVector4Array] = shape.get_samples_3d(1.0);
	add_samples_as_lines(world_space_samples, gizmo, get_material("main", gizmo));

static func add_samples_as_lines(samples: Array[PackedVector4Array], gizmo: EditorNode3DGizmo, material: StandardMaterial3D) -> void:
	var lines := PackedVector3Array()
	for child_array: PackedVector4Array in samples:
		for i in range(child_array.size() - 1):
			var sample: Vector4 = child_array[i]
			var next_sample: Vector4 = child_array[i + 1]
			if (sample.w > 0 && next_sample.w > 0):
				lines.push_back(Vector3(sample.x, sample.y, sample.z))
				if next_sample == sample:
					# This is a dot, so make it a small line so it's at least visible
					next_sample += Vector4(0.01, 0.01, 0.01, 0.0) # TODO: make this better
				lines.push_back(Vector3(next_sample.x, next_sample.y, next_sample.z))
	gizmo.add_lines(lines, material, false)
