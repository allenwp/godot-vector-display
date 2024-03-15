extends Node3D

@export var speed: float = 0.01;
@export var around_z: bool = false #if true, it will rotate around y

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(_delta: float) -> void:
	if around_z:
		rotate_z(speed);
	else:
		rotate_y(speed);

