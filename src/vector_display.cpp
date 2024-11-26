#include "vector_display.h"
#include <godot_cpp/core/class_db.hpp>
#include "output/vd_frame_output.h"
#include "godot_cpp/classes/tween.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "vd_shape_3d.h"
#include "vd_renderer.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "vd_editor_preview_3d.h"
#include "vd_post_processor_3d.h"
#include "vd_post_processor_2d.h"
#include <chrono>
#include <thread>

using namespace godot;
using namespace vector_display;

void VectorDisplay::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start_asio_output"), &VectorDisplay::start_asio_output);
	ClassDB::bind_method(D_METHOD("is_output_running"), &VectorDisplay::is_output_running);

	ClassDB::bind_method(D_METHOD("get_previous_refresh_time"), &VectorDisplay::get_previous_frame_time);

	ClassDB::bind_method(D_METHOD("debug_get_last_starved_samples"), &VectorDisplay::get_last_starved_samples);
	ClassDB::bind_method(D_METHOD("debug_get_process_time"), &VectorDisplay::debug_get_process_time);

	ClassDB::bind_method(D_METHOD("debug_reset_asio_profiling"), &VectorDisplay::reset_asio_profiling);
	ClassDB::bind_method(D_METHOD("debug_get_asio_min_time_between_buffer_switch"), &VectorDisplay::get_asio_min_time_between_buffer_switch);
	ClassDB::bind_method(D_METHOD("debug_get_asio_max_time_between_buffer_switch"), &VectorDisplay::get_asio_max_time_between_buffer_switch);
	ClassDB::bind_method(D_METHOD("debug_get_asio_min_time_to_copy_buffers"), &VectorDisplay::get_asio_min_time_to_copy_buffers);
	ClassDB::bind_method(D_METHOD("debug_get_asio_max_time_to_copy_buffers"), &VectorDisplay::get_asio_max_time_to_copy_buffers);
	ClassDB::bind_method(D_METHOD("debug_get_asio_min_headroom"), &VectorDisplay::debug_get_asio_min_headroom);
	ClassDB::bind_method(D_METHOD("debug_get_asio_max_headroom"), &VectorDisplay::debug_get_asio_max_headroom);
}

VectorDisplay::VectorDisplay() {
	Variant vsync_setting = ProjectSettings::get_singleton()->get_setting("display/window/vsync/vsync_mode");
	if (vsync_setting.get_type() != Variant::Type::INT || (int)vsync_setting != 0) {
		ERR_PRINT_ED("V-Sync is not disabled! V-Sync must be disabled to use the VectorDisplay Node.");
	}
	reset_buffers();
}

VectorDisplay::~VectorDisplay() {
	if (output) {
		delete output;
		output = nullptr;
	}
}

void VectorDisplay::start_asio_output() {
	if (output) {
		delete output;
	}
	output = new VDASIOOutput();

	reset_buffers();
}

void VectorDisplay::reset_buffers() {
	WriteState = WriteStateEnum::Buffer1;
	previousFinalSample = VDSampleHelper::GetBlankingSample();
	VDSample *buffer = nullptr;
	buffer = VDFrameOutput::Buffer1.load(std::memory_order_acquire);
	VDFrameOutput::Buffer1.store(nullptr, std::memory_order_release);
	if (buffer != nullptr) {
		delete buffer;
	}
	buffer = VDFrameOutput::Buffer2.load(std::memory_order_acquire);
	VDFrameOutput::Buffer2.store(nullptr, std::memory_order_release);
	if (buffer != nullptr) {
		delete buffer;
	}
}

void VectorDisplay::_ready() {
	set_process_priority(INT32_MAX);
}

//double value = 0;
void VectorDisplay::_process(double delta) {
	TypedArray<PackedVector4Array> worldSpaceResult = TypedArray<PackedVector4Array>();
	TypedArray<PackedVector3Array> screenSpaceSamples = GetScreenSpaceSamples(worldSpaceResult);

	// Finally, prepare and fill the FrameOutput buffer:
	int blankingSampleCount;
	int wastedSampleCount;
	int finalBufferLength;
	VDSample *finalBuffer = CreateFrameBuffer(screenSpaceSamples, previousFinalSample, blankingSampleCount, wastedSampleCount, finalBufferLength); // VDFrameOutput::GetCalibrationFrame(finalBufferLength);
	if (finalBufferLength > 0) {
		previousFinalSample = finalBuffer[finalBufferLength - 1];
	}
	previousFrameTime = (double)finalBufferLength / VDFrameOutput::DACProfile->samples_per_second;

	//// Debug test code to simulate tricky double buffer situations
	//RandomNumberGenerator rand;
	//if (rand.randi_range(0, 60) == 0) {
	//	UtilityFunctions::print("Sleeping to simulate a long frame time.");
	//	Sleep(8);
	//}

	if (VDFrameOutput::DebugSaveFrame) {
		VDFrameOutput::DebugSaveBufferToFile(finalBuffer, finalBufferLength, "Frame Snapshot.csv");
		VDFrameOutput::DebugSaveFrame = false;
		output->DebugSaveNextFrame = true;
	}

	int64_t frame_ready_timestamp = VDFrameOutput::get_ticks_now();
	debug_process_time = VDFrameOutput::get_ms_from_ticks(frame_ready_timestamp - debug_process_timestamp);

	// Wait for the output to be finished with the buffer we're about to write to
	if (!output) {
		// No need to limit the framerate if there isn't an output consuming the frames.
		VDSample *oldBuffer = nullptr;
		if (WriteState == WriteStateEnum::Buffer1) {
			oldBuffer = VDFrameOutput::Buffer1.load(std::memory_order_relaxed);
		}
		if (WriteState == WriteStateEnum::Buffer2) {
			oldBuffer = VDFrameOutput::Buffer2.load(std::memory_order_relaxed);
		}
		if (oldBuffer != nullptr) {
			delete oldBuffer;
		}
	} else {
		while ((WriteState == WriteStateEnum::Buffer1 && VDFrameOutput::Buffer1.load(std::memory_order_acquire) != nullptr) || (WriteState == WriteStateEnum::Buffer2 && VDFrameOutput::Buffer2.load(std::memory_order_acquire) != nullptr)) {
			YieldProcessor();
		}
	}

	debug_process_timestamp = VDFrameOutput::get_ticks_now();

	// Assign the buffer and progress the frame buffer write state
	if (WriteState == WriteStateEnum::Buffer1) {
		VDFrameOutput::DebugBuffer1Timestamp = frame_ready_timestamp;
		VDFrameOutput::Buffer1Length = finalBufferLength;
		VDFrameOutput::Buffer1.store(finalBuffer, std::memory_order_release);
		WriteState = WriteStateEnum::Buffer2;
	} else {
		VDFrameOutput::DebugBuffer2Timestamp = frame_ready_timestamp;
		VDFrameOutput::Buffer2Length = finalBufferLength;
		VDFrameOutput::Buffer2.store(finalBuffer, std::memory_order_release);
		WriteState = WriteStateEnum::Buffer1;
	}

	// VDFrameOutput::StarvedSamples is written to on a different thread, so just grab a snapshot.
	// It doesn't matter if this is out of date because we'll grab another snapshot next frame.
	unsigned int starvedSamplesSnapshot = VDFrameOutput::StarvedSamples;
	thisFrameStarvedSamples = starvedSamplesSnapshot - totalStarvedSamples;
	totalStarvedSamples = starvedSamplesSnapshot;

	VDFrameOutput::FrameCount++;

	if (VDFrameOutput::FrameCount % 100 == 0) {
		// TODO: write framerate?
		//int frameRate = (int)round(1 / ((float)GameTime.LastFrameSampleCount / VDFrameOutput::SAMPLES_PER_SECOND));
		//Console.WriteLine(" " + finalBuffer.Length + " + " + starvedSamples + " starved samples = " + frameRate + " fps (" + blankingSampleCount + " blanking between shapes, " + wastedSampleCount + " wasted) | Frame worst: " + frameTimePerf.worst + " best: " + frameTimePerf.best + " avg: " + frameTimePerf.average + " | Output Sync longest: " + syncOverheadTime.worst + " shortest: " + syncOverheadTime.best + " avg: " + syncOverheadTime.average + " | Host worst: " + hostTimePerf.worst + " best: " + hostTimePerf.best + " avg: " + hostTimePerf.average);
	}

	if (Engine::get_singleton()->is_editor_hint()) {
		TypedArray<Node> children = get_children();
		for (int i = 0; i < children.size(); i++) {
			Node* child = (Node*)((Object*)children[i]);
			if (child->is_class("VDEditorPreview3D")) {
				VDEditorPreview3D *editor_preview = (VDEditorPreview3D*)child;
				editor_preview->update_preview(worldSpaceResult);
			}
		}
	}
}

// This function is similar to the original SamplerSystem::Tick() method
TypedArray<PackedVector3Array> VectorDisplay::GetScreenSpaceSamples(TypedArray<PackedVector4Array> &worldSpaceResult) {
	TypedArray<PackedVector3Array> result;

	Window *root = get_tree()->get_root();
	Camera3D *camera = get_viewport()->get_camera_3d();
	// TODO: for each camera
	if (camera != nullptr && camera->is_visible_in_tree()) {
		TypedArray<Node> shapeNodes = root->find_children("*", "VDShape3D", true, false); // owned must be false, but I don't understand why.

		worldSpaceResult = TypedArray<PackedVector4Array>();

		for (int i = 0; i < shapeNodes.size(); i++) {
			VDShape3D *shape = Object::cast_to<VDShape3D>(shapeNodes[i]);
			if (shape) {
				// TODO: multi-thread this:
				if (!VDRenderer::ShouldCull(camera, shape)) {
					worldSpaceResult.append_array(VDRenderer::GetSample3Ds(camera, shape));
				}
			}
		}

		TypedArray<Node> children = camera->get_children();
		for (int i = 0; i < children.size(); i++) {
			VDPostProcessor3D *pp = Object::cast_to<VDPostProcessor3D>(children[i]);
			if (pp && pp->can_process()) {
				pp->process_samples_3d(worldSpaceResult);
			}
		}

		// World space samples are now ready to be translated to the screen!
		TypedArray<PackedVector3Array> screenSpaceResult = VDRenderer::TransformSamples3DToScreen(camera, worldSpaceResult);

		children = camera->get_children();
		for (int i = 0; i < children.size(); i++) {
			VDPostProcessor2D *pp = Object::cast_to<VDPostProcessor2D>(children[i]);
			if (pp && pp->can_process()) {
				pp->process_samples_2d(screenSpaceResult);
			}
		}

		result.append_array(screenSpaceResult);
	}

	// Global post processing applies after all per-camera post processing
	TypedArray<Node> global_pp_roots = root->find_children("*", "VDGlobalPostProcessingRoot", true, false); // owned must be false, but I don't understand why.
	for (int i = 0; i < global_pp_roots.size(); i++) {
		Node* pp_root = Object::cast_to<Node>(global_pp_roots[i]);
		TypedArray<Node> children = pp_root->get_children();
		for (int j = 0; j < children.size(); j++) {
			VDPostProcessor2D *pp = Object::cast_to<VDPostProcessor2D>(children[j]);
			if (pp && pp->can_process()) {
				pp->process_samples_2d(result);
			}
		}
	}

	return result;
}

VDSample *VectorDisplay::CreateFrameBuffer(TypedArray<PackedVector3Array> samples, VDSample previousFrameEndSample, int &blankingSamplesOut, int &wastedSamplesOut, int &bufferLengthOut) {
	// Sorting (Disabled code)
	// Sorting has the advantage of reducing beam overshooting between shapes.
	// It is disabled because it makes it worse! Here's why:
	// When shapes are sorted, they draw in a different order between frames.
	// This means that during one frame, a shape may be the first to be drawn
	// ...but on the next frame, they might be the last to be drawn.
	// In this case, it causes the shape to start flickering as if the refresh
	// rate is too low. I discovered this behavour immediately started happening
	// when I enabled this sorting method.
	//
	// With this knowledge in mind, I actually think it is best to draw shapes in
	// the exact same order every frame, regardless of their position on the screen
	// in order to reduce flicker and give a smooth video. This means overshooting
	// will happen, but it's a less distracting problem than the flicker and jitter.
	//
	//// Remove all empty sample arrays
	//samples.RemoveAll(delegate(Sample[] array) {
	//	return array.Length < 1;
	//});
	//
	//// Sort based on a starting point of Sample.Blank
	//List<Sample[]> sortedSamples = new List<Sample[]>(samples.Count);
	//Sample beamPosition = Sample.Blank;
	//while (samples.Count > 1)
	//{
	//    samples.Sort(delegate (Sample[] array1, Sample[] array2)
	//    {
	//        float distanceTo1 = DistanceBetweenSamples(beamPosition, array1[0]);
	//        float distanceTo2 = DistanceBetweenSamples(beamPosition, array2[0]);
	//        if (distanceTo1 < distanceTo2)
	//        {
	//            return -1;
	//        }
	//        else if (distanceTo1 > distanceTo2)
	//        {
	//            return 1;
	//        }
	//        else
	//        {
	//            return 0;
	//        }
	//    });
	//    beamPosition = samples[0][0];
	//    sortedSamples.Add(samples[0]);
	//    samples.Remove(samples[0]);
	//}
	//sortedSamples.Add(samples[0]);
	//
	//samples = sortedSamples;

	// Find out how many samples we have in the full set
	int sampleCount = 0;
	for (int i = 0; i < samples.size(); i++) {
		sampleCount += (int)(samples[i].call("size"));
	}
	int worstCaseBlankingLength = VDFrameOutput::DACProfile->BlankingLength(VDSample(-1.0f, -1.0f, 0.0f), VDSample(1.0f, 1.0f, 0.0f));
	int worstCaseSampleCount = sampleCount + (samples.size() * worstCaseBlankingLength) + worstCaseBlankingLength + 1; // one more on the end to return to blanking point.
	worstCaseSampleCount = MAX(VDFrameOutput::GetTargetBufferSize(), worstCaseSampleCount);

	VDSample* finalBuffer = new VDSample[worstCaseSampleCount];

	// Copy the full set of samples into the final buffer:
	int destinationIndex = 0;
	VDSample previousSample = previousFrameEndSample;

	auto addSamples = [&](Variant sampleArray) { // sampleArray must be of type PackedVector3Array
		int sampleArraySize = (int)(sampleArray.call("size"));
		if (sampleArraySize > 0) {
			bool valid, oob;
			VDSample firstSample = sampleArray.get_indexed(0, valid, oob);
			int blankingLength = VDFrameOutput::DACProfile->BlankingLength(previousSample, firstSample);
			// Set blanking based on the first sample:
			for (int b = 0; b < blankingLength; b++) {
				VDSample tweenSample;
				// Ease in b/c the preceeding samples might have given the beam momentum
				// Ease out b/c the following samples should start without too much momentum
				// from movement during blanking.
				float tweenValue = EaseInOutPower((b + 1) / (float)blankingLength, 2);
				tweenSample.x = Math::lerp(previousSample.x, firstSample.x, tweenValue);
				tweenSample.y = Math::lerp(previousSample.y, firstSample.y, tweenValue);
				VD_SAMPLE_BRIGHTNESS(tweenSample) = 0.0f;

				finalBuffer[destinationIndex] = tweenSample;
				destinationIndex++;
			}

			// Then copy the samples over:
			for (int i = 0; i < sampleArraySize; i++) {
				finalBuffer[destinationIndex + i] = sampleArray.get_indexed(i, valid, oob);
			}
			destinationIndex += sampleArraySize;
			previousSample = sampleArray.get_indexed(sampleArraySize - 1, valid, oob);
		}
	};

	for (int i = 0; i < samples.size(); i++) {
		addSamples(samples[i]);
	}

	int finalSampleCount = destinationIndex;
	if (finalSampleCount < VDFrameOutput::GetTargetBufferSize()) {
		// Since we're going to be blanking for the end of this frame, we need to do the same easeinout blanking before we get to the rest postition.
		PackedVector3Array newSampleArray;
		newSampleArray.push_back(VDSampleHelper::GetBlankingSample());
		addSamples(newSampleArray);
		finalSampleCount = destinationIndex;
	}

	// Set up the final buffer with the correct sample length after dynamic blanking has been performed
	// This is variable (variable frame rate based on paramenters in FrameOutput class)
	blankingSamplesOut = finalSampleCount - sampleCount;
	bufferLengthOut = finalSampleCount;
	if (finalSampleCount < VDFrameOutput::GetTargetBufferSize()) {
		bufferLengthOut = VDFrameOutput::GetTargetBufferSize(); // we know that finalBuffer is at least this large.
		// Only in this case to we need to clear the last bits of the buffer.
		// In the other cases it's already filled to bufferLengthOut
		VDFrameOutput::ClearBuffer(finalBuffer, bufferLengthOut, finalSampleCount);
	}
	wastedSamplesOut = bufferLengthOut - finalSampleCount;

	return finalBuffer;
}

float VectorDisplay::EaseInOutPower(float progress, int power) {
	progress *= 2;
	if (progress < 1) {
		return (float)Math::pow(progress, power) / 2.0f;
	} else {
		int sign = power % 2 == 0 ? -1 : 1;
		return (float)(sign / 2.0 * (Math::pow(progress - 2, power) + sign * 2));
	}
}

bool VectorDisplay::is_output_running() {
	return output != nullptr;
}

double VectorDisplay::get_previous_frame_time() {
	return previousFrameTime;
}

int VectorDisplay::get_last_starved_samples() {
	return thisFrameStarvedSamples;
}

void vector_display::VectorDisplay::reset_asio_profiling() {
	if (output != nullptr) {
		output->_reset_profiling = true;
	}
}

double VectorDisplay::get_asio_min_time_between_buffer_switch() {
	if (output != nullptr) {
		return output->minTimeBetweenBufferSwitch;
	} else {
		return 9999;
	}
}

double vector_display::VectorDisplay::get_asio_max_time_between_buffer_switch() {
	if (output != nullptr) {
		return output->maxTimeBetweenBufferSwitch;
	} else {
		return 0;
	}
}

double vector_display::VectorDisplay::get_asio_min_time_to_copy_buffers() {
	if (output != nullptr) {
		return output->minTimeToCopyBuffers;
	} else {
		return 9999;
	}
}

double vector_display::VectorDisplay::get_asio_max_time_to_copy_buffers() {
	if (output != nullptr) {
		return output->maxTimeToCopyBuffers;
	} else {
		return 0;
	}
}

double vector_display::VectorDisplay::debug_get_asio_min_headroom() {
	if (output != nullptr) {
		return output->minHeadroom;
	} else {
		return 0;
	}
}

double vector_display::VectorDisplay::debug_get_asio_max_headroom() {
	if (output != nullptr) {
		return output->maxHeadroom;
	} else {
		return 0;
	}
}

double vector_display::VectorDisplay::debug_get_process_time() {
	return debug_process_time;
}
