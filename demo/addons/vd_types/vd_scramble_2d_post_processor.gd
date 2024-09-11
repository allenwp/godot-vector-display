@tool
class_name Scramble2dPostProcessor extends VDPostProcessor2D

@export var amount: Vector2 = Vector2.ZERO

func _process_samples_2d(samples: Array[PackedVector3Array]) -> void:
	if amount != Vector2.ZERO:
		for samplesArray in samples:
			for i in range(samplesArray.size()):
				var sample: Vector3 = samplesArray[i]
				sample.x += (randf() - 0.5) * amount.x
				sample.y += (randf() - 0.5) * amount.y
				samplesArray[i] = sample
