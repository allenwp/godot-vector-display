#ifndef VD_RENDERER_H
#define VD_RENDERER_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <vd_shape_3d.h>
#include <godot_cpp/variant/transform3d.hpp>

namespace vector_display {

class VDRenderer  {

private:
protected:

public:
	static TypedArray<Array> GetSample3Ds(Camera3D *camera, VDShape3D *shape);
	static TypedArray<PackedVector3Array> TransformSamples3DToScreen(Camera3D *camera, const TypedArray<Array> samples3D);

	/// <summary>
	/// Produces "view" coordinates (aka "eye" or "camera" coordinates)
	/// Applies the view transform of the given camera (transformCamera.worldToCamera)
	/// to the homogeonous vertex.
	/// </summary>
	static Vector3 PerformViewTransform(Vector3 vertex, Transform3D worldToCamera);

	/// <summary>
	/// Produces "clip" coordinates by applying the projection transform.
	/// This result can be transformed into "Normalized Device Coordinates" by performing a homogeneous divide.
	/// </summary>
	static Vector4 PerformProjectionTransform(Vector4 vertex, Projection projection);

	static bool Clip(Vector4 vertex);

	/// <summary>
	/// Performs a homogeneous divide and discards z to get the final screen space coordinates
	/// </summary>
	static Vector2 PerformViewportTransform(Vector4 vertex, bool scaleToAspectRatio, float aspectRatio);
};

} // namespace vector_display

#endif