extends Node3D

var ring:PackedScene = preload("res://space_canine_clone/environment_objects/ring.tscn")
var spire:PackedScene = preload("res://space_canine_clone/environment_objects/spire.tscn")

var filled_in_to: float = 0

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	filled_in_to = global_position.z

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	if floorf(global_position.z / 100) < floorf(filled_in_to / 100):
		filled_in_to = global_position.z

		for i in range(-200, 200, 50):
			var value: float = randf()
			var new_obj: EnvironmentObject

			if value < 0.5:
				new_obj = spire.instantiate()
			else:
				new_obj = ring.instantiate()

			get_tree().root.add_child(new_obj)
			new_obj.global_position = global_position
			new_obj.global_position.x = i + randf_range(-20, 20)
			new_obj.global_position.y +=randf_range(-20, 20)
			new_obj.global_position.z +=randf_range(-20, 20)
			new_obj.dolly = %Dolly

