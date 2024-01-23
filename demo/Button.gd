extends Button

@onready var vector_display: VectorDisplay = %VectorDisplay
@onready var spin_box: SpinBox = %SpinBox

func _on_pressed() -> void:
	vector_display.start_asio_output(spin_box.value);
