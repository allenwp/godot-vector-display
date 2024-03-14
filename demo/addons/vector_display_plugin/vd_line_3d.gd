#class_name VDLine3D extends VDShape3D
#
#@export var start: Vector3
#@export var end: Vector3
#@export var base_sample_count: int = 50
#
#func _get_samples_3d(fidelity: float) -> Array[Array]:
	#var result: Array[Array]
	#var sampleLength: int = roundi(base_sample_count * fidelity)
#
	#var sample3DArray: Array[Vector4]
	#sample3DArray.resize(sampleLength)
	#for i in range(sampleLength):
		#var point3D: Vector3 = lerp(start, end, (i as float) / ((sampleLength - 1) as float))
		#var sample: Vector4 = Vector4(point3D.x, point3D.y, point3D.z, 1)
		#sample3DArray[i] = sample
	#result.push_back(sample3DArray)
#
	#return result
