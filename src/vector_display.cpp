#include "vector_display.h"
#include <godot_cpp/core/class_db.hpp>
#include "output/vd_frame_output.h"
#include "godot_cpp/classes/tween.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;
using namespace vector_display;

void VectorDisplay::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start_asio_output"), &VectorDisplay::start_asio_output);
}

VectorDisplay::VectorDisplay() {
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

//double value = 0;
void VectorDisplay::_process(double delta) {
	TypedArray<Array> screenSpaceSamples = GetScreenSpaceSamples();

	// Finally, prepare and fill the FrameOutput buffer:
	int blankingSampleCount;
	int wastedSampleCount;
	int finalBufferLength;
	VDSample *finalBuffer = CreateFrameBuffer(screenSpaceSamples, previousFinalSample, blankingSampleCount, wastedSampleCount, finalBufferLength); // VDFrameOutput::GetCalibrationFrame();
	if (finalBufferLength > 0) {
		previousFinalSample = finalBuffer[finalBufferLength - 1];
	}

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
		}
	}

	// Assign the buffer and progress the frame buffer write state
	if (WriteState == WriteStateEnum::Buffer1) {
		VDFrameOutput::Buffer1Length = finalBufferLength;
		VDFrameOutput::Buffer1.store(finalBuffer, std::memory_order_release);
		WriteState = WriteStateEnum::Buffer2;
	} else {
		VDFrameOutput::Buffer2Length = finalBufferLength;
		VDFrameOutput::Buffer2.store(finalBuffer, std::memory_order_release);
		WriteState = WriteStateEnum::Buffer1;
	}

	// This part regarding the number of starved samples is not thread perfect, but I think it should be
	// correct more than 99.9% of the time... And if not, it doesn't really matter.
	int starvedSamples = VDFrameOutput::StarvedSamples;
	VDFrameOutput::StarvedSamples = 0;

	VDFrameOutput::FrameCount++;

	if (VDFrameOutput::FrameCount % 100 == 0) {
		// TODO: write framerate?
		//int frameRate = (int)round(1 / ((float)GameTime.LastFrameSampleCount / VDFrameOutput::SAMPLES_PER_SECOND));
		//Console.WriteLine(" " + finalBuffer.Length + " + " + starvedSamples + " starved samples = " + frameRate + " fps (" + blankingSampleCount + " blanking between shapes, " + wastedSampleCount + " wasted) | Frame worst: " + frameTimePerf.worst + " best: " + frameTimePerf.best + " avg: " + frameTimePerf.average + " | Output Sync longest: " + syncOverheadTime.worst + " shortest: " + syncOverheadTime.best + " avg: " + syncOverheadTime.average + " | Host worst: " + hostTimePerf.worst + " best: " + hostTimePerf.best + " avg: " + hostTimePerf.average);
	}
}

TypedArray<Array> VectorDisplay::GetScreenSpaceSamples() {
	TypedArray<Array> result;



	return result;
}

VDSample *VectorDisplay::CreateFrameBuffer(TypedArray<Array> samples, VDSample previousFrameEndSample, int &blankingSamplesOut, int &wastedSamplesOut, int &bufferLengthOut) {
	// TODO:
	// Remove all empty sample arrays
	//samples.RemoveAll(delegate(Sample[] array) {
	//	return array.Length < 1;
	//});

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

	//samples = sortedSamples;

	// Find out how many samples we have in the full set
	int sampleCount = 0;
	for (int i = 0; i < samples.size(); i++) {
		TypedArray<VDSample> sampleArray = samples[i];
		sampleCount += sampleArray.size();
	}
	int worstCaseBlankingLength = VDFrameOutput::DisplayProfile->BlankingLength(VDSample(-1.0f, -1.0f, 0.0f), VDSample(1.0f, 1.0f, 0.0f));
	int worstCaseSampleCount = sampleCount + (samples.size() * worstCaseBlankingLength) + worstCaseBlankingLength + 1; // one more on the end to return to blanking point.
	worstCaseSampleCount = MAX(VDFrameOutput::GetTargetBufferSize(), worstCaseSampleCount);

	VDSample* finalBuffer = new VDSample[worstCaseSampleCount];

	// Copy the full set of samples into the final buffer:
	int destinationIndex = 0;
	VDSample previousSample = previousFrameEndSample;

	auto addSamples = [&](TypedArray<VDSample> sampleArray) {
		int blankingLength = VDFrameOutput::DisplayProfile->BlankingLength(previousSample, sampleArray[0]);
		// Set blanking based on the first sample:
		for (int b = 0; b < blankingLength; b++) {
			VDSample tweenSample;
			// Ease in b/c the preceeding samples might have given the beam momentum
			// Ease out b/c the following samples should start without too much momentum
			// from movement during blanking.
			float tweenValue = EaseInOutPower((b + 1) / (float)blankingLength, 2);
			tweenSample.x = Math::lerp(previousSample.x, ((VDSample)sampleArray[0]).x, tweenValue);
			tweenSample.y = Math::lerp(previousSample.y, ((VDSample)sampleArray[0]).y, tweenValue);
			VD_SAMPLE_BRIGHTNESS(tweenSample) = 0.0f;

			finalBuffer[destinationIndex] = tweenSample;
			destinationIndex++;
		}

		// Then copy the samples over:
		for (int i = 0; i < sampleArray.size(); i++) {
			finalBuffer[destinationIndex + i] = sampleArray[i];
		}
		destinationIndex += sampleArray.size();
		previousSample = sampleArray[sampleArray.size() - 1];
	};

	for (int i = 0; i < samples.size(); i++) {
		TypedArray<VDSample> sampleArray = samples[i];
		addSamples(sampleArray);
	}

	int finalSampleCount = destinationIndex;
	if (finalSampleCount < VDFrameOutput::GetTargetBufferSize()) {
		// Since we're going to be blanking for the end of this frame, we need to do the same easeinout blanking before we get to the rest postition.
		TypedArray<VDSample> newSampleArray;
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