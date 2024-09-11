@tool
class_name VDCurlyCircle extends VDShape3D

@export var CurlCount: int = 20
@export var CurlSize: float = 0.2
@export var AnimationSpeed: float = 0.1
@export var AnimationOffset: float = 0
@export var base_sample_count: int = 3000

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _physics_process(_delta: float) -> void:
	AnimationOffset += AnimationSpeed / 100

func _get_samples_3d(fidelity: float) -> Array[PackedVector4Array]:
	var result: Array[PackedVector4Array]

	var sampleCount: int = roundi(base_sample_count * fidelity)

	var sample3DArray: PackedVector4Array
	sample3DArray.resize(sampleCount)
	for i in range(sampleCount):
		var progress: float = (i as float) / (sampleCount - 1)
		progress = ease_out_power(progress, 3) + AnimationOffset

		var value: float = lerpf(0, PI * 2, progress)
		var point3D: Vector3 = Vector3(sin(value), cos(value), 0);
#
		var secondValue: float = lerpf(0, PI * 2, progress * CurlCount);
		var secondCirclePoint: Vector3 = CurlSize * Vector3(sin(secondValue), cos(secondValue), 0);
		point3D += secondCirclePoint;
#
		sample3DArray[i] = Vector4(point3D.x, point3D.y, point3D.z, 1)
	result.push_back(sample3DArray)
	return result

static func ease_out_power(progress: float, power: int) -> float:
	var pos_neg_sign: int = -1 if power % 2 == 0 else 1
	return pos_neg_sign * pow(progress - 1, power) + pos_neg_sign
