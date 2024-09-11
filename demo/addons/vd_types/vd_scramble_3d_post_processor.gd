@tool
class_name Scramble3dPostProcessor extends VDPostProcessor3D

@export var amount: Vector3 = Vector3.ZERO

func _process_samples_3d(samples_3d: Array[PackedVector4Array]) -> void:
	if amount != Vector3.ZERO:
		for samplesArray in samples_3d:
			for i in range(samplesArray.size()):
				var sample: Vector4 = samplesArray[i]
				sample.x += (randf() - 0.5) * amount.x
				sample.y += (randf() - 0.5) * amount.y
				sample.z += (randf() - 0.5) * amount.z
				samplesArray[i] = sample
