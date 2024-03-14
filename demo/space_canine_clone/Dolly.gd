class_name Dolly
extends Node3D

@export var speed: float = 1

@export var bounds: Vector2 = Vector2(100, 40)
@export var ground: float = -15

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(delta: float) -> void:
	position += (self.quaternion * Vector3.FORWARD) * speed * delta;
