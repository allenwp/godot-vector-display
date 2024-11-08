extends Node3D

var counted_time: float = 0
var total_time: float = 0

func _process(delta: float) -> void:
	total_time += delta
	counted_time += delta
	if counted_time > 10:
		counted_time -= 10
		print("seconds: " + str(total_time))
