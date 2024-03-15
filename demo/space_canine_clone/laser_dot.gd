class_name LaserDot extends VDDot3D

@onready var laser: Laser = $".."
var initial_sample_count: int

func _ready() -> void:
	initial_sample_count = sample_count

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	var fade_start_distance: float = 200
	var current_distance: float = laser.initial_position.distance_to(global_position)
	if current_distance > fade_start_distance:
		var value: float = (current_distance - fade_start_distance) / (laser.max_distance - fade_start_distance)
		value = clampf(value, 0.0, 1.0)
		sample_count = lerp(initial_sample_count, 0, value)
