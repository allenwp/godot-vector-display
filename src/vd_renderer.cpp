#include "vd_renderer.h"
#include "output/vd_frame_output.h"

using namespace godot;
using namespace vector_display;

// returns TypedArray<TypedArray<Vector4>> // TODO: Change to TypedArray<PackedVector4Array>
TypedArray<Array> VDRenderer::GetSample3Ds(Camera3D *camera, VDShape3D *shape) {
	float fidelity;
	if (camera->get_projection() == Camera3D::PROJECTION_PERSPECTIVE) {
		float distanceFromCamera = abs(shape->get_global_position().distance_to(camera->get_global_position()));
		float minDistanceFromCamera = camera->get_near();
		if (distanceFromCamera < minDistanceFromCamera) {
			distanceFromCamera = minDistanceFromCamera;
		}
		// Fidelity is relative to the "non-3D" plane equivalent being the distance where half of the camera's FoV shows 1 unit on an axis,
		// which matches the coordinate space that this engine uses, which is -1 to 1 (or 1 unit for half the screen)
		// For a fidelity of 1, the distance from camera must equal (1 / (tan(FoV / 2))) ("TOA" triginometry formula)
		// The first 1 in the following equation is based on half of the camera's vision being 1 unit of screen space ("TOA" triginometry formula)
		// This formula uses the half of the camera's vision being 1 unit to match up with drawing the shape as non-3D
		double fov_rad = Math::deg_to_rad(camera->get_fov());
		fidelity = 1.0 / (distanceFromCamera * (float)tan(fov_rad / 2.0));
	} else if (camera->get_projection() == Camera3D::PROJECTION_ORTHOGONAL) {
		fidelity = 1.0;
	} else {
		WARN_PRINT_ED("Unsupported camera type");
	}

	fidelity *= VDFrameOutput::DisplayProfile->FidelityScale;

	Vector3 shape_scale = shape->get_scale();
	fidelity *= MAX(MAX(abs(shape_scale.x), abs(shape_scale.y)), abs(shape_scale.z)); // Multiply fidelity by max scale

	// Now we have the fidelity for this shape. Get the samples:
	TypedArray<Array> samples3D = shape->get_samples_3d(fidelity);

	// TODO: should this post processing be done on the shape after local transform has been applied??
	// I feel like... no???

	// TODO: local shape post processing
	//// Post process the local space samples:
	//var shapePostProcessor3D = shape.Entity.GetComponent<PostProcessing.PostProcessingGroup3D>();
	//if (shapePostProcessor3D != null) {
	//	foreach (var postProcessor in shapePostProcessor3D.PostProcessors.Where(comp = > comp.IsActive)) {
	//		postProcessor.PostProcess3DFuntion(samples3D, postProcessor);
	//	}
	//}

	// Transform the samples into world space and record them in the sample stream:
	shape->apply_global_transform(samples3D);
	return samples3D;
}

// samples3D is TypedArray<TypedArray<VDSample3D>> // TODO: Change to TypedArray<PackedVector4Array>
TypedArray<PackedVector3Array> VDRenderer::TransformSamples3DToScreen(Camera3D *camera,  const TypedArray<Array> samples3D) {
	TypedArray<PackedVector3Array> result = TypedArray<PackedVector3Array>();
	for (int j = 0; j < samples3D.size(); j++) {
		const Array samples3DArray = samples3D[j]; // must be contained in an Array instead of a TypedArray because of https://github.com/godotengine/godot/issues/89191
		int sampleLength = samples3DArray.size();

		PackedVector3Array tempSampleArray = PackedVector3Array();
		int currentArrayIndex = 0;
		for (int i = 0; i < sampleLength; i++) {
			VDSample3D worldSample = samples3DArray[i];
			Vector3 worldPos = Vector3(worldSample.x, worldSample.y, worldSample.z);
			Vector4 v4;
			// When samples are 0 brightness (disabled), it's the same as when they're clipped
			bool clipped = VD_SAMPLE_3D_BRIGHTNESS(worldSample) == 0.0f;
			if (!clipped) {
				worldPos = PerformViewTransform(worldPos, camera->get_camera_transform());
				v4 = Vector4(worldPos.x, worldPos.y, worldPos.z, 1);
				v4 = PerformProjectionTransform(v4, camera->get_camera_projection());
				clipped = Clip(v4);
			} else {
				v4 = Vector4(worldPos.x, worldPos.y, worldPos.z, 1);
			}

			if (!clipped) {
				if (tempSampleArray.size() == 0) {
					tempSampleArray.resize(sampleLength);
				}

				Vector2 result2D = PerformViewportTransform(v4, true, VDFrameOutput::DisplayProfile->AspectRatio);
				tempSampleArray[currentArrayIndex] = VDSample(result2D.x, result2D.y, VD_SAMPLE_3D_BRIGHTNESS(worldSample));

				currentArrayIndex++;
			} else {
				// Move on to the next array of samples. This allows blanking to be applied in the case where
				// a continuous path exits the screen on one side and comes in on the other side of the screen.
				if (tempSampleArray.size() > 0 && currentArrayIndex > 0) {
					tempSampleArray.resize(currentArrayIndex);
					result.push_back(tempSampleArray);
				}
				tempSampleArray = PackedVector3Array(); // discard old one
				currentArrayIndex = 0;
			}
		}

		if (tempSampleArray.size() > 0 && currentArrayIndex > 0) {
			tempSampleArray.resize(currentArrayIndex);
			result.push_back(tempSampleArray);
		}
	}

	return result;
}

Vector3 VDRenderer::PerformViewTransform(Vector3 vertex, Transform3D worldToCamera) {
	return worldToCamera.xform(vertex);
}

Vector4 VDRenderer::PerformProjectionTransform(Vector4 vertex, Projection projection) {
	return projection.xform(vertex);
}

bool VDRenderer::Clip(Vector4 vertex) {
	bool result = !(-vertex.w <= vertex.x && vertex.x <= vertex.w
		&& -vertex.w <= vertex.y && vertex.y <= vertex.w
		&& -vertex.w <= vertex.z && vertex.z <= vertex.w);
	return result;
}

Vector2 VDRenderer::PerformViewportTransform(Vector4 vertex, bool scaleToAspectRatio, float aspectRatio) {
	Vector2 viewportSpace(vertex.x / vertex.w, vertex.y / vertex.w);

	if (scaleToAspectRatio) {
		viewportSpace.x *= aspectRatio;
	}

	return viewportSpace;
}