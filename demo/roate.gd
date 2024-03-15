extends Node3D

@export var speed: float = 0.01;

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(_delta: float) -> void:
	rotate_y(speed);
