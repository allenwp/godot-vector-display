extends Node3D
class_name Laser

@export var speed: float = 0.2
@export var max_distance: float = 1000

var initial_position: Vector3 = Vector3.ZERO

func _ready() -> void:
	initial_position = position

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(_delta: float) -> void:
	var direction := Vector3.FORWARD * quaternion.inverse()
	position += direction * speed

	if initial_position.distance_to(global_position) > max_distance:
		queue_free()
