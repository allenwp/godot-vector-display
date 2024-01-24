extends Button

@onready var vector_display: VectorDisplay = %VectorDisplay

func _on_pressed() -> void:
	vector_display.start_asio_output();
