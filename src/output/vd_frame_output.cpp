#include "vd_frame_output.h"
#include "godot_cpp/core/math.hpp"
#include "vd_display_profile_Osc_Tek_TAS_465.h"
#include "vd_display_profile_Osc_Tek_2445.h"
#include "vd_dac_profile_presonus_studio_26c.h"
#include "output/vd_asio_output.h" // for QueryPerformanceFrequency. This could be improved.

using namespace vector_display;

float VDFrameOutput::MaxFramesPerSecond = 80.0f;
VDDisplayProfile *VDFrameOutput::DisplayProfile = (VDDisplayProfile*)new VDDisplayProfileOscTekTAS465();
VDDACProfile *VDFrameOutput::DACProfile = (VDDACProfile*)new VDDACProfilePresonusStudio26c();

std::atomic<VDSample *> VDFrameOutput::Buffer1 = nullptr;
int VDFrameOutput::Buffer1Length = 0;
int64_t VDFrameOutput::DebugBuffer1Timestamp = 0;
std::atomic<VDSample *> VDFrameOutput::Buffer2 = nullptr;
int VDFrameOutput::Buffer2Length = 0;
int64_t VDFrameOutput::DebugBuffer2Timestamp = 0;
unsigned int VDFrameOutput::StarvedSamples = 0;
unsigned long VDFrameOutput::FrameCount = 0;
bool VDFrameOutput::DebugSaveFrame = false;

VDFrameOutput::VDFrameOutput() {
}

VDFrameOutput::~VDFrameOutput() {
}

int VDFrameOutput::GetTargetBufferSize() {
	int result = (int)roundf(DACProfile->samples_per_second / MaxFramesPerSecond);
	if (result < 1) {
		result = 1;
	}
	return result;
}

void VDFrameOutput::ClearBuffer(VDSample *buffer, int bufferLength, int startIndex) {
	VDSample blank = VDSampleHelper::GetBlankingSample();
	for (int i = startIndex; i < bufferLength; i++) {
		buffer[i] = blank;
	}
}

void VDFrameOutput::DebugSaveBufferToFile(VDSample* buffer, int bufferLength, const char* path) {
	// TODO:
	//StringBuilder sb = new StringBuilder();
	//sb.AppendLine("X,Y,Brightness");
	//for (int i = 0; i < bufferLength; i++) {
	//	sb.AppendLine($ "{buffer[i].X:R},{buffer[i].Y:R},{buffer[i].Brightness:R}");
	//}
	//File.WriteAllText(path, sb.ToString());
}

//TODO : Move calibration stuff to a separate project
VDSample* VDFrameOutput::GetCalibrationFrame(int& bufferLengthOut) {
	int brightestSampleCount = 300;

	bufferLengthOut = brightestSampleCount * 8;
	VDSample *buffer = new VDSample[bufferLengthOut];
	ClearBuffer(buffer, bufferLengthOut, 0);

	int bufferIndex = 0;

	float aspectRatio = DisplayProfile->AspectRatio;

	bufferIndex = CalibrationDrawPoint(-aspectRatio, -1, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(-aspectRatio, 0, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(-aspectRatio, 1, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(0, 1, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(aspectRatio, 1, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(aspectRatio, 0, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(aspectRatio, -1, buffer, brightestSampleCount, bufferIndex);
	bufferIndex = CalibrationDrawPoint(0, -1, buffer, brightestSampleCount, bufferIndex);

	return buffer;
}

/// <returns>Next safe index to write to</returns>
int VDFrameOutput::CalibrationDrawPoint(float x, float y, VDSample* buffer, int numSamples, int startIndex) {
	int finalIndexPlusOne = startIndex + numSamples;
	for (; startIndex < finalIndexPlusOne; startIndex++) {
		buffer[startIndex] = VDSample(x, y, 1.0f);
	}
	return finalIndexPlusOne;
}

int64_t VDFrameOutput::get_ticks_now() {
	LARGE_INTEGER ticks;
	if (!QueryPerformanceCounter(&ticks)) {
		ticks.QuadPart = 0;
	}
	return ticks.QuadPart;
}

double VDFrameOutput::get_ms_from_ticks(int64_t ticks) {
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency); 
	ticks *= 1000000; // Guard against loss-of-precision by converting to microseconds *before* dividing by ticks-per-second.
	int64_t usec = ticks / Frequency.QuadPart;
	return usec / (double)1000.0;
}
