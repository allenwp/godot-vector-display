#ifndef VD_FRAME_OUTPUT_H
#define VD_FRAME_OUTPUT_H

#include "vd_sample.h"
#include "vd_display_profile.h"

namespace vector_display {

class VDFrameOutput {
private:
protected:
public:
	VDFrameOutput();
	~VDFrameOutput();

	static int const SAMPLES_PER_SECOND = 192000;

	/// <summary>
	/// My PreSonus DAC has a delay on the channel I use for blanking -_-
	/// </summary>
	static int const BLANKING_CHANNEL_DELAY = 18;

	/// <summary>
	/// Recommended: 60 to 100. Default: 80.
	/// Setting this too high may result in the CPU not being able to provide new frames fast enough.
	/// Setting this too low will cause flickering.
	/// </summary>
	static float MaxFramesPerSecond;

	static VDDisplayProfile* DisplayProfile;

	/// <summary>
	/// GameLoop checks to see if it's null before writting.
	/// Output should set to null when it's finished reading.
	/// </summary>
	static std::atomic<VDSample *> Buffer1;
	static std::atomic<int> Buffer1Length;
	/// <summary>
	/// GameLoop checks to see if it's null before writting.
	/// Output should set to null when it's finished reading.
	/// </summary>
	static std::atomic<VDSample *> Buffer2;
	static std::atomic<int> Buffer2Length;

	static int StarvedSamples;

	/// <summary>
	/// This will eventually overflow and loop back to 0.
	/// </summary>
	static unsigned long FrameCount;

	static bool DebugSaveFrame;

	static int GetTargetBufferSize();

	static void ClearBuffer(VDSample *buffer, int bufferLength, int startIndex = 0);
	
	static void DebugSaveBufferToFile(VDSample *buffer, int bufferLength, const char *path);

	static VDSample *GetCalibrationFrame(int &bufferLengthOut);

	/// <returns>Next safe index to write to</returns>
	static int CalibrationDrawPoint(float x, float y, VDSample *buffer, int numSamples, int startIndex);
};

} // namespace vector_display

#endif