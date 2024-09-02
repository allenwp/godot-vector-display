@tool
class_name VDScriptLine3D extends VDShape3D

@export var start: Vector3
@export var end: Vector3
@export var base_sample_count: int = 50

func _get_samples_3d(fidelity: float) -> Array[PackedVector4Array]:
	var result: Array[PackedVector4Array]
	var sampleLength: int = roundi(base_sample_count * fidelity)

	var sample3DArray: PackedVector4Array
	sample3DArray.resize(sampleLength)
	for i in range(sampleLength):
		var point3D: Vector3 = lerp(start, end, (i as float) / ((sampleLength - 1) as float))
		var sample: Vector4 = Vector4(point3D.x, point3D.y, point3D.z, 1)
		sample3DArray[i] = sample
	result.push_back(sample3DArray)

	return result
