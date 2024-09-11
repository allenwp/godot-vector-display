extends Node

## If null, get_parent() will be used instead.
@export var target: Node3D = null

@export var speed: float = 0.01;
@export var around_z: bool = false #if true, it will rotate around y

func _physics_process(_delta: float) -> void:
	var node_3d: Node3D
	if target != null:
		node_3d = target
	else:
		var parent: Node = get_parent()
		if parent is Node3D:
			node_3d = parent as Node3D

	if node_3d == null:
		printerr("Rotate script requires a target or parent Node3D.")
	else:
		var scaled_speed: float = speed / 100
		if around_z:
			node_3d.rotate_z(scaled_speed)
		else:
			node_3d.rotate_y(scaled_speed);
