class_name VDCrossfadePostProcessor
extends VDPostProcessor2D

# USAGE:
# When processing mode is active, this post processor will scale the brightness
# by the value property. Use the animation system to animate crossfades.

@export var value: float = 1.0

func _process_samples_2d(samples: Array[PackedVector3Array]) -> void:
	if value != 1.0:
		for samples_array: PackedVector3Array in samples:
			for i: int in range(samples_array.size()):
				var sample := samples_array[i]
				sample.z *= value
				samples_array[i] = sample
