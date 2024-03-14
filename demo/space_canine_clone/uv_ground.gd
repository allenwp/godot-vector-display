extends MeshInstance3D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	# TODO: grab the rotation and roate the global position around it somehow to make this work
	# when the dolly rotates
	var globalPos := global_position
	var meshSize := (mesh as PlaneMesh).size
	var mat:BaseMaterial3D = mesh.surface_get_material(0)
	var uvScale := mat.uv1_scale
	# 1000 is mesh size in gobal coordinates. 25 is UV 1
	mat.uv1_offset = Vector3(globalPos.x / (meshSize.x / uvScale.x), globalPos.z / (meshSize.y / uvScale.y), 0)
