#ifndef VD_FRAME_OUTPUT_H
#define VD_FRAME_OUTPUT_H

#include "vd_sample.h"
#include "vd_display_profile.h"
#include "vd_dac_profile.h"

namespace vector_display {

class VDFrameOutput {
private:
protected:
public:
	VDFrameOutput();
	~VDFrameOutput();

	/// <summary>
	/// Recommended: 60 to 100. Default: 80.
	/// Setting this too high may result in the CPU not being able to provide new frames fast enough.
	/// Setting this too low will cause flickering.
	/// </summary>
	static float MaxFramesPerSecond;

	static VDDisplayProfile* DisplayProfile;
	static VDDACProfile *DACProfile;

	/// <summary>
	/// Output should delete and set to null when it's finished reading.
	/// VectorDisplay::_process waits until it's null before writting.
	/// </summary>
	static std::atomic<VDSample *> Buffer1;
	static int Buffer1Length;
	/// <summary>
	/// Output should delete and set to null when it's finished reading.
	/// VectorDisplay::_process waits until it's null before writting.
	/// </summary>
	static std::atomic<VDSample *> Buffer2;
	static int Buffer2Length;

	static unsigned int StarvedSamples;

	/// <summary>
	/// This will eventually overflow and loop back to 0.
	/// </summary>
	static unsigned long FrameCount;

	static bool DebugSaveFrame;

	static int GetTargetBufferSize();

	static void ClearBuffer(VDSample *buffer, int bufferLength, int startIndex);
	
	static void DebugSaveBufferToFile(VDSample *buffer, int bufferLength, const char *path);

	static VDSample *GetCalibrationFrame(int &bufferLengthOut);

	/// <returns>Next safe index to write to</returns>
	static int CalibrationDrawPoint(float x, float y, VDSample *buffer, int numSamples, int startIndex);

	static int64_t get_ticks_now();
	static double get_ms_from_ticks(int64_t ticks);
};

} // namespace vector_display

#endif