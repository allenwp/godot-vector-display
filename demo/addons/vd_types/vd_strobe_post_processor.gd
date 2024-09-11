@tool
class_name VDStrobePostProcessor extends VDPostProcessor3D

@export var AnimationValue: float = 0
@export var AnimationSpeed: float = 1
@export var Scale: float = 25

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(_delta: float) -> void:
	AnimationValue += AnimationSpeed / 100
	while AnimationValue > 1:
		AnimationValue -= 1

func _process_samples_3d(samples_3d: Array[PackedVector4Array]) -> void:
	var animationValue: float = AnimationValue * PI * 2
	for samples3DArray: PackedVector4Array in samples_3d:
		for i in range(samples3DArray.size()):
			var sample: Vector4 = samples3DArray[i]
			var sinValue: float = sin(sample.y * Scale + animationValue)
			if sinValue < 0:
				# TODO: does this work? samples3DArray[i].w = 0;
				sample.w = 0
				samples3DArray[i] = sample
