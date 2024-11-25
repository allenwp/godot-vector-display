extends ColorRect

@export var speed:float = 0.1

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if is_visible_in_tree():
		var vd: VectorDisplay = GlobalVectorDisplay
		position.x += vd.get_previous_frame_time() * speed
		if (position.x > 1000):
			position.x = 0
