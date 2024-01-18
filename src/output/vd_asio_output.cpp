#include "vd_asio_output.h"
#include "godot_cpp/core/math.hpp"
#include "vd_frame_output.h"

using namespace godot;
using namespace vector_display;

extern AsioDrivers *asioDrivers;

VDASIOOutput* VDASIOOutput::_static_instance = nullptr;

VDASIOOutput::VDASIOOutput() {
	if (_static_instance != nullptr) {
		throw "Trying to create a new VDASIOOutput, but one already exists!";
	}
	_static_instance = this;

	asioDrivers = new AsioDrivers();
	char *driverName = new char[32];
	asioDrivers->getDriverNames(&driverName, 1);

	blankingChannelDelayBufferLength = VDFrameOutput::BLANKING_CHANNEL_DELAY;
	blankingChannelDelayBuffer = new float[blankingChannelDelayBufferLength];
	ReadState = ReadStateEnum::Buffer1;

	// TODO: This could spin on a different thread to monitor ASIO driver state,
	// similar to the SDK sample's while loop.

	bool loaded = false;
	if (asioDrivers->loadDriver(driverName)) {
		// initialize the driver
		if (ASIOInit(&asioDriverInfo.driverInfo) == ASE_OK) {
			//printf("asioVersion:   %d\n"
			//	   "driverVersion: %d\n"
			//	   "Name:          %s\n"
			//	   "ErrorMessage:  %s\n",
			//		asioDriverInfo.driverInfo.asioVersion, asioDriverInfo.driverInfo.driverVersion,
			//		asioDriverInfo.driverInfo.name, asioDriverInfo.driverInfo.errorMessage);
			if (init_asio_static_data(&asioDriverInfo) == 0) {
				ASIOControlPanel(); //you might want to check wether the ASIOControlPanel() can open

				// set up the asioCallback structure and create the ASIO data buffer
				asioCallbacks.bufferSwitch = &bufferSwitch;
				asioCallbacks.sampleRateDidChange = &sampleRateChanged;
				asioCallbacks.asioMessage = &asioMessages;
				asioCallbacks.bufferSwitchTimeInfo = &bufferSwitchTimeInfo;
				if (create_asio_buffers(&asioDriverInfo) == ASE_OK) {
					if (ASIOStart() == ASE_OK) {
						_need_to_stop = true;
						loaded = true;
					}
					_need_to_dispose_buffers = true;
				}
			}
			_need_to_exit = true;
		}
		_need_to_remove_current_driver = true;
	}

	if (!loaded) {
		_cleanup();
	}

	delete driverName;
}

VDASIOOutput::~VDASIOOutput() {
	_cleanup();
	_static_instance = nullptr;
}

void VDASIOOutput::_cleanup() {
	if (_need_to_stop) {
		ASIOStop();
		_need_to_stop = false;
	}
	if (_need_to_dispose_buffers) {
		ASIODisposeBuffers();
		_need_to_dispose_buffers = false;
	}
	if (_need_to_exit) {
		ASIOExit();
		_need_to_exit = false;
	}
	if (_need_to_remove_current_driver) {
		asioDrivers->removeCurrentDriver();
		_need_to_remove_current_driver = false;
	}
	if (asioDrivers) {
		delete asioDrivers;
		asioDrivers = nullptr;
	}
	if (blankingChannelDelayBuffer) {
		delete blankingChannelDelayBuffer;
		blankingChannelDelayBuffer = nullptr;
	}
}

long VDASIOOutput::init_asio_static_data(DriverInfo *asioDriverInfo) { // collect the informational data of the driver
	// get the number of available channels
	if (ASIOGetChannels(&asioDriverInfo->inputChannels, &asioDriverInfo->outputChannels) == ASE_OK) {
		//printf("ASIOGetChannels (inputs: %d, outputs: %d);\n", asioDriverInfo->inputChannels, asioDriverInfo->outputChannels);

		// get the usable buffer sizes
		if (ASIOGetBufferSize(&asioDriverInfo->minSize, &asioDriverInfo->maxSize, &asioDriverInfo->preferredSize, &asioDriverInfo->granularity) == ASE_OK) {
			//printf("ASIOGetBufferSize (min: %d, max: %d, preferred: %d, granularity: %d);\n",
					//asioDriverInfo->minSize, asioDriverInfo->maxSize,
					//asioDriverInfo->preferredSize, asioDriverInfo->granularity);

			// get the currently selected sample rate
			if (ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK) {
				//printf("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);
				if (asioDriverInfo->sampleRate != 192000.0) {
					// Driver does not store it's internal sample rate, so set it to a know one.
					// Usually you should check beforehand, that the selected sample rate is valid
					// with ASIOCanSampleRate().
					if (ASIOSetSampleRate(192000.0) == ASE_OK) {
						if (ASIOGetSampleRate(&asioDriverInfo->sampleRate) == ASE_OK) {
							//printf("ASIOGetSampleRate (sampleRate: %f);\n", asioDriverInfo->sampleRate);
						}
						else
							return -6;
					} else
						return -5;
				}

				// check wether the driver requires the ASIOOutputReady() optimization
				// (can be used by the driver to reduce output latency by one block)
				if (ASIOOutputReady() == ASE_OK)
					asioDriverInfo->postOutput = true;
				else
					asioDriverInfo->postOutput = false;
				//printf("ASIOOutputReady(); - %s\n", asioDriverInfo->postOutput ? "Supported" : "Not supported");

				return 0;
			}
			return -3;
		}
		return -2;
	}
	return -1;
}

//----------------------------------------------------------------------------------
// conversion from 64 bit ASIOSample/ASIOTimeStamp to double float
#if NATIVE_INT64
#define ASIO64toDouble(a) (a)
#else
const double twoRaisedTo32 = 4294967296.;
#define ASIO64toDouble(a) ((a).lo + (a).hi * twoRaisedTo32)
#endif

ASIOTime *VDASIOOutput::bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow) { // the actual processing callback.
	// Beware that this is normally in a seperate thread, hence be sure that you take care
	// about thread synchronization. This is omitted here for simplicity.

	if (!processNow) {
		throw "This ASIO driver does not support direct processing in its buffer fill callback thread. This is not currently supported by the Vector Display library, but might actually be fine. (It needs testing!)";
	}

	// store the timeInfo for later use
	_static_instance->asioDriverInfo.tInfo = *timeInfo;

	// get the time stamp of the buffer, not necessary if no
	// synchronization to other media is required
	if (timeInfo->timeInfo.flags & kSystemTimeValid)
		_static_instance->asioDriverInfo.nanoSeconds = ASIO64toDouble(timeInfo->timeInfo.systemTime);
	else
		_static_instance->asioDriverInfo.nanoSeconds = 0;

	if (timeInfo->timeInfo.flags & kSamplePositionValid)
		_static_instance->asioDriverInfo.samples = ASIO64toDouble(timeInfo->timeInfo.samplePosition);
	else
		_static_instance->asioDriverInfo.samples = 0;

	if (timeInfo->timeCode.flags & kTcValid)
		_static_instance->asioDriverInfo.tcSamples = ASIO64toDouble(timeInfo->timeCode.timeCodeSamples);
	else
		_static_instance->asioDriverInfo.tcSamples = 0;

	// get the system reference time
	_static_instance->asioDriverInfo.sysRefTime = get_sys_reference_time();

#if WINDOWS && _DEBUG
	// a few debug messages for the Windows device driver developer
	// tells you the time when driver got its interrupt and the delay until the app receives
	// the event notification.
	static double last_samples = 0;
	//char tmp[128];
	//sprintf(tmp, "diff: %d / %d ms / %d ms / %d samples                 \n", _static_output->asioDriverInfo.sysRefTime - (long)(_static_output->asioDriverInfo.nanoSeconds / 1000000.0), _static_output->asioDriverInfo.sysRefTime, (long)(_static_output->asioDriverInfo.nanoSeconds / 1000000.0), (long)(_static_output->asioDriverInfo.samples - last_samples));
	//OutputDebugString(tmp);
	last_samples = _static_instance->asioDriverInfo.samples;
#endif

	// buffer size in samples
	long buffSize = _static_instance->asioDriverInfo.preferredSize;

	float *xOutput = new float[buffSize];
	float *yOutput = new float[buffSize];
	float *zOutput = new float[buffSize];

	_static_instance->FeedFloatBuffers(xOutput, yOutput, zOutput, buffSize, 0);

	if (_static_instance->DebugSaveThisFrame) {
		DebugSaveBuffersToFile(xOutput, yOutput, zOutput, "ASIO Frame Snapshot (float).csv");
	}

	_static_instance->ApplyBlankingChannelDelay(zOutput, buffSize);

	if (_static_instance->DebugSaveThisFrame) {
		DebugSaveBuffersToFile(xOutput, yOutput, zOutput, "ASIO Frame Snapshot (Blanking Delay Applied) (float).csv");
		_static_instance->DebugSaveThisFrame = false;
	}

#if DEBUG_CODE
	// Currently in C#; still needs to be transcoded:
	// checking for delays between channels
	//for (int i = 0; i < xOutput.BufferSize; i++)
	//{
	//    yOutput[i] = xOutput[i];
	//    zOutput[i] = xOutput[i];
	//}

	// Code for a test tone to make sure ASIO device is working well:
	//for (int index = 0; index < xOutput.BufferSize; index++)
	//{
	//	t += 0.03;
	//	xOutput[index] = (float)Math.Sin(t);
	//	yOutput[index] = (float)Math.Sin(t);
	//	zOutput[index] = (float)Math.Sin(t);
	//}

	// Code for testing blanking:
	//for (int index = 0; index < xOutput.BufferSize; index++)
	//{
	//	t = Microsoft.Xna.Framework.MathHelper.Lerp(0, (float)Math.PI, (float)index / xOutput.BufferSize);
	//	xOutput[index] = (float)Math.Sin(t);
	//	yOutput[index] = (float)Math.Cos(t);

	//	if (yOutput[index] > 0.5f || yOutput[index] < -0.5f)
	//	{
	//		zOutput[index] = 1f;// (float)Math.Sin(t);
	//	}
	//	else
	//	{
	//		zOutput[index] = 0f;// (float)Math.Sin(t);
	//	}
	//}

	// Code for debugging flicker to 0,0:
	//for (int i = 0; i < xOutput.BufferSize; i++)
	//{
	//	double threshold = 0.01;
	//	if (Math.Abs((double)xOutput[i]) < threshold && Math.Abs((double)yOutput[i]) < threshold)
	//	{
	//		Console.WriteLine("output is 0");
	//	}
	//}

	//// Code for debugging oscilloscope faulty(?) X input that looks like it's not properly DC Coupled
	//frameCount++;
	//if (frameCount % 300 == 0)
	//{
	//	high = !high;
	//}
	//for (int i = 0; i < xOutput.BufferSize; i++)
	//{
	//	float value;
	//	if (i < xOutput.BufferSize / 2)
	//	{
	//		value = high ? 0.65f : -0.65f;
	//	}
	//	else
	//	{
	//		value = high ? 0.1f : -0.1f;
	//	}
	//	xOutput[i] = value;
	//	yOutput[i] = value;
	//	zOutput[i] = 0f;
	//}

	//// Alternative code for debugging oscilloscope faulty(?) X input that looks like it's not properly DC Coupled
	//frameCount++;
	//if (frameCount % 50 == 0)
	//{
	//	high = !high;
	//}
	//for (int i = 0; i < xOutput.BufferSize; i++)
	//{
	//	float value;
	//	value = high ? 0.1f : -0.1f;
	//	xOutput[i] = value;
	//	yOutput[i] = value;
	//	zOutput[i] = 0f;
	//}

	//// Alternative code for debugging oscilloscope faulty(?) X input that looks like it's not properly DC Coupled
	//frameCount++;
	//if (frameCount % 50 == 0)
	//{
	//	high = !high;
	//}
	//for (int i = 0; i < xOutput.BufferSize; i++)
	//{
	//	float value = -0.5f;
	//	if (high && i > xOutput.BufferSize / 3 && i < (xOutput.BufferSize / 3) * 2)
	//	{
	//		value = 0.5f;
	//	}
	//	xOutput[i] = value;
	//	zOutput[i] = 0f;
	//}
	//for (int i = 0; i < yOutput.BufferSize; i++)
	//{
	//	yOutput[i] = (float)i / (yOutput.BufferSize - 1);
	//	yOutput[i] = yOutput[i] * 2f - 1f;
	//	zOutput[i] = 0f;
	//}
#endif

	// fill the final output buffers
	int outputIndex = 0;
	for (int i = 0; i < _static_instance->asioDriverInfo.inputBuffers + _static_instance->asioDriverInfo.outputBuffers; i++) {
		if (_static_instance->asioDriverInfo.bufferInfos[i].isInput == false) {
			void *driverBuffer = _static_instance->asioDriverInfo.bufferInfos[i].buffers[index];

			float *floatBuffer = nullptr;
			switch (outputIndex) {
				case 0:
					floatBuffer = zOutput;
					break;
				case 2:
					floatBuffer = xOutput;
					break;
				case 3:
					floatBuffer = yOutput;
					break;
				case 1:
					floatBuffer = xOutput; // Just for demo purposes. This line can be removed.
				default:
					// leave the buffer nullptr to indicate that this channel should be filled with silence
					break;
			}

			switch (_static_instance->asioDriverInfo.channelInfos[i].type) {
				case ASIOSTInt16LSB:
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 2);
					}
					break;
				case ASIOSTInt24LSB: // used for 20 bits as well
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 3);
					}
					break;
				case ASIOSTInt32LSB:
					if (floatBuffer) {
						long *castBuffer = static_cast<long *>(driverBuffer);
						const double fScaler32 = (double)0x7fffffffL;
						double sc = fScaler32 + .49999;
						for (int sampleIndex = 0; sampleIndex < buffSize; sampleIndex++) {
							castBuffer[sampleIndex] = (long)((double)floatBuffer[sampleIndex] * sc);
						}
					} else {
						memset(driverBuffer, 0, buffSize * 4);
					}
					break;
				case ASIOSTFloat32LSB: // IEEE 754 32 bit float, as found on Intel x86 architecture
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 4);
					}
					break;
				case ASIOSTFloat64LSB: // IEEE 754 64 bit double float, as found on Intel x86 architecture
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 8);
					}
					break;

					// these are used for 32 bit data buffer, with different alignment of the data inside
					// 32 bit PCI bus systems can more easily used with these
				case ASIOSTInt32LSB16: // 32 bit data with 18 bit alignment
				case ASIOSTInt32LSB18: // 32 bit data with 18 bit alignment
				case ASIOSTInt32LSB20: // 32 bit data with 20 bit alignment
				case ASIOSTInt32LSB24: // 32 bit data with 24 bit alignment
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 4);
					}
					break;

				case ASIOSTInt16MSB:
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 2);
					}
					break;
				case ASIOSTInt24MSB: // used for 20 bits as well
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 3);
					}
					break;
				case ASIOSTInt32MSB:
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 4);
					}
					break;
				case ASIOSTFloat32MSB: // IEEE 754 32 bit float, as found on Intel x86 architecture
					if (floatBuffer) {
						float *castBuffer = static_cast<float*>(driverBuffer);
						for (int sampleIndex = 0; sampleIndex < buffSize; sampleIndex++) {
							castBuffer[sampleIndex] = floatBuffer[sampleIndex];
						}
					} else {
						memset(driverBuffer, 0, buffSize * 4);
					}
					break;
				case ASIOSTFloat64MSB: // IEEE 754 64 bit double float, as found on Intel x86 architecture
					if (floatBuffer) {
						double *castBuffer = static_cast<double *>(driverBuffer);
						for (int sampleIndex = 0; sampleIndex < buffSize; sampleIndex++) {
							castBuffer[sampleIndex] = (double)floatBuffer[sampleIndex];
						}
					} else {
						memset(driverBuffer, 0, buffSize * 8);
					}
					break;

					// these are used for 32 bit data buffer, with different alignment of the data inside
					// 32 bit PCI bus systems can more easily used with these
				case ASIOSTInt32MSB16: // 32 bit data with 18 bit alignment
				case ASIOSTInt32MSB18: // 32 bit data with 18 bit alignment
				case ASIOSTInt32MSB20: // 32 bit data with 20 bit alignment
				case ASIOSTInt32MSB24: // 32 bit data with 24 bit alignment
					if (floatBuffer) {
						// TODO when needed
					} else {
						memset(driverBuffer, 0, buffSize * 4);
					}
					break;
			}
			outputIndex++;
		}
	}

	// finally if the driver supports the ASIOOutputReady() optimization, do it here, all data are in place
	if (_static_instance->asioDriverInfo.postOutput)
		ASIOOutputReady();

	delete xOutput;
	delete yOutput;
	delete zOutput;

	return 0L;
}

//----------------------------------------------------------------------------------
void VDASIOOutput::bufferSwitch(long index, ASIOBool processNow) { // the actual processing callback.
	// Beware that this is normally in a seperate thread, hence be sure that you take care
	// about thread synchronization. This is omitted here for simplicity.

	// as this is a "back door" into the bufferSwitchTimeInfo a timeInfo needs to be created
	// though it will only set the timeInfo.samplePosition and timeInfo.systemTime fields and the according flags
	ASIOTime timeInfo;
	memset(&timeInfo, 0, sizeof(timeInfo));

	// get the time stamp of the buffer, not necessary if no
	// synchronization to other media is required
	if (ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition, &timeInfo.timeInfo.systemTime) == ASE_OK)
		timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;

	bufferSwitchTimeInfo(&timeInfo, index, processNow);
}

//----------------------------------------------------------------------------------
void VDASIOOutput::sampleRateChanged(ASIOSampleRate sRate) {
	// do whatever you need to do if the sample rate changed
	// usually this only happens during external sync.
	// Audio processing is not stopped by the driver, actual sample rate
	// might not have even changed, maybe only the sample rate status of an
	// AES/EBU or S/PDIF digital input at the audio device.
	// You might have to update time/sample related conversion routines, etc.
}

//----------------------------------------------------------------------------------
long VDASIOOutput::asioMessages(long selector, long value, void *message, double *opt) {
	// currently the parameters "value", "message" and "opt" are not used.
	long ret = 0;
	switch (selector) {
		case kAsioSelectorSupported:
			if (value == kAsioResetRequest || value == kAsioEngineVersion || value == kAsioResyncRequest || value == kAsioLatenciesChanged
					// the following three were added for ASIO 2.0, you don't necessarily have to support them
					|| value == kAsioSupportsTimeInfo || value == kAsioSupportsTimeCode || value == kAsioSupportsInputMonitor)
				ret = 1L;
			break;
		case kAsioResetRequest:
			// defer the task and perform the reset of the driver during the next "safe" situation
			// You cannot reset the driver right now, as this code is called from the driver.
			// Reset the driver is done by completely destruct is. I.e. ASIOStop(), ASIODisposeBuffers(), Destruction
			// Afterwards you initialize the driver again.
			_static_instance->asioDriverInfo.stopped; // In this sample the processing will just stop
			ret = 1L;
			break;
		case kAsioResyncRequest:
			// This informs the application, that the driver encountered some non fatal data loss.
			// It is used for synchronization purposes of different media.
			// Added mainly to work around the Win16Mutex problems in Windows 95/98 with the
			// Windows Multimedia system, which could loose data because the Mutex was hold too long
			// by another thread.
			// However a driver can issue it in other situations, too.
			ret = 1L;
			break;
		case kAsioLatenciesChanged:
			// This will inform the host application that the drivers were latencies changed.
			// Beware, it this does not mean that the buffer sizes have changed!
			// You might need to update internal delay data.
			ret = 1L;
			break;
		case kAsioEngineVersion:
			// return the supported ASIO version of the host application
			// If a host applications does not implement this selector, ASIO 1.0 is assumed
			// by the driver
			ret = 2L;
			break;
		case kAsioSupportsTimeInfo:
			// informs the driver wether the asioCallbacks.bufferSwitchTimeInfo() callback
			// is supported.
			// For compatibility with ASIO 1.0 drivers the host application should always support
			// the "old" bufferSwitch method, too.
			ret = 1;
			break;
		case kAsioSupportsTimeCode:
			// informs the driver wether application is interested in time code info.
			// If an application does not need to know about time code, the driver has less work
			// to do.
			ret = 0;
			break;
	}
	return ret;
}

//----------------------------------------------------------------------------------
ASIOError VDASIOOutput::create_asio_buffers(DriverInfo *asioDriverInfo) { // create buffers for all inputs and outputs of the card with the
	// preferredSize from ASIOGetBufferSize() as buffer size
	long i;
	ASIOError result;

	// fill the bufferInfos from the start without a gap
	ASIOBufferInfo *info = asioDriverInfo->bufferInfos;

	// prepare inputs (Though this is not necessaily required, no opened inputs will work, too
	if (asioDriverInfo->inputChannels > kMaxInputChannels)
		asioDriverInfo->inputBuffers = kMaxInputChannels;
	else
		asioDriverInfo->inputBuffers = asioDriverInfo->inputChannels;
	for (i = 0; i < asioDriverInfo->inputBuffers; i++, info++) {
		info->isInput = ASIOTrue;
		info->channelNum = i;
		info->buffers[0] = info->buffers[1] = 0;
	}

	// prepare outputs
	if (asioDriverInfo->outputChannels > kMaxOutputChannels)
		asioDriverInfo->outputBuffers = kMaxOutputChannels;
	else
		asioDriverInfo->outputBuffers = asioDriverInfo->outputChannels;
	for (i = 0; i < asioDriverInfo->outputBuffers; i++, info++) {
		info->isInput = ASIOFalse;
		info->channelNum = i;
		info->buffers[0] = info->buffers[1] = 0;
	}

	// create and activate buffers
	result = ASIOCreateBuffers(asioDriverInfo->bufferInfos,
			asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers,
			asioDriverInfo->preferredSize, &asioCallbacks);
	if (result == ASE_OK) {
		// now get all the buffer details, sample word length, name, word clock group and activation
		for (i = 0; i < asioDriverInfo->inputBuffers + asioDriverInfo->outputBuffers; i++) {
			asioDriverInfo->channelInfos[i].channel = asioDriverInfo->bufferInfos[i].channelNum;
			asioDriverInfo->channelInfos[i].isInput = asioDriverInfo->bufferInfos[i].isInput;
			result = ASIOGetChannelInfo(&asioDriverInfo->channelInfos[i]);
			if (result != ASE_OK)
				break;
		}

		if (result == ASE_OK) {
			// get the input and output latencies
			// Latencies often are only valid after ASIOCreateBuffers()
			// (input latency is the age of the first sample in the currently returned audio block)
			// (output latency is the time the first sample in the currently returned audio block requires to get to the output)
			result = ASIOGetLatencies(&asioDriverInfo->inputLatency, &asioDriverInfo->outputLatency);
			if (result == ASE_OK) {
				//printf("ASIOGetLatencies (input: %d, output: %d);\n", asioDriverInfo->inputLatency, asioDriverInfo->outputLatency);
			}
		}
	}
	return result;
}

unsigned long VDASIOOutput::get_sys_reference_time() { // get the system reference time
#if WINDOWS
	return timeGetTime();
#elif MAC
	static const double twoRaisedTo32 = 4294967296.;
	UnsignedWide ys;
	Microseconds(&ys);
	double r = ((double)ys.hi * twoRaisedTo32 + (double)ys.lo);
	return (unsigned long)(r / 1000.);
#endif
}

void VDASIOOutput::FeedFloatBuffers(float *xOutput, float *yOutput, float *brightnessOutput, int bufferSize, int startIndex) {
	VDSample *currentFrameBuffer = nullptr;
	int currentFrameBufferLength = 0;
	if (ReadState == ReadStateEnum::Buffer1) {
		currentFrameBuffer = VDFrameOutput::Buffer1;
		currentFrameBufferLength = VDFrameOutput::Buffer1Length;
	} else {
		currentFrameBuffer = VDFrameOutput::Buffer2;
		currentFrameBufferLength = VDFrameOutput::Buffer2Length;
	}

	if (currentFrameBuffer == nullptr) {
		int blankedSampleCount = bufferSize - startIndex;
		VDFrameOutput::StarvedSamples += blankedSampleCount;
		// TODO: Console.WriteLine("AUDIO BUFFER IS STARVED FOR FRAMES! Blanking for " + blankedSampleCount);
		// Clear the rest of the buffer with blanking frames
		for (int i = startIndex; i < bufferSize; i++) {
			// TODO: this should probably just pause on the last position instead (which might be blanking position, but might not be)
			xOutput[i] = -1.0f;
			yOutput[i] = -1.0f;
			brightnessOutput[i] = VDFrameOutput::DisplayProfile->ZeroBrightnessOutput;
		}
		return;
	}

	for (int i = startIndex; i < bufferSize; i++) {
		// Move to the next buffer if needed by recursively calling this method:
		if (frameIndex >= currentFrameBufferLength) {
			CompleteFrame();
			FeedFloatBuffers(xOutput, yOutput, brightnessOutput, bufferSize, i);
			return;
		}

		VDSample adjustedSample = PrepareSampleForScreen(currentFrameBuffer[frameIndex]);
		xOutput[i] = adjustedSample.x;
		yOutput[i] = adjustedSample.y;
		brightnessOutput[i] = VD_SAMPLE_BRIGHTNESS(adjustedSample);

		frameIndex++;
	}

	if (frameIndex >= currentFrameBufferLength) {
		CompleteFrame();
	}
}

void VDASIOOutput::CompleteFrame() {
	switch (ReadState) {
		case ReadStateEnum::Buffer1:
			VDFrameOutput::Buffer1 = nullptr;
			ReadState = ReadStateEnum::Buffer2;
			break;
		case ReadStateEnum::Buffer2:
			VDFrameOutput::Buffer2 = nullptr;
			ReadState = ReadStateEnum::Buffer1;
			break;
	}

	frameIndex = 0;

	if (DebugSaveNextFrame) {
		DebugSaveNextFrame = false;
		DebugSaveThisFrame = true;
	}
}

VDSample VDASIOOutput::PrepareSampleForScreen(VDSample sample) {
	float aspectRatio = VDFrameOutput::DisplayProfile->AspectRatio;
	if (aspectRatio > 1.0f) {
		sample.x /= aspectRatio;
		sample.y /= aspectRatio;
	} else {
		// Nothing to do with a portrait or square aspect ratio
		// In these cases, Y is already in range of -1 to 1 and
		// X is whatever range it needs to be to match the aspect ratio.
	}

	VD_SAMPLE_BRIGHTNESS(sample) = Math::lerp(VDFrameOutput::DisplayProfile->ZeroBrightnessOutput, VDFrameOutput::DisplayProfile->FullBrightnessOutput, Math::clamp(VD_SAMPLE_BRIGHTNESS(sample), 0.0f, 1.0f));

	return sample;
}

void VDASIOOutput::ApplyBlankingChannelDelay(float *blankingChannel, int bufferLength) {
	float* originalStream = new float[bufferLength];
	for (int i = 0; i < bufferLength; i++) {
		originalStream[i] = blankingChannel[i];
	}
	for (int i = 0; i < bufferLength; i++) {
		if (i < blankingChannelDelayBufferLength) {
			blankingChannel[i] = blankingChannelDelayBuffer[i];
		} else {
			blankingChannel[i] = originalStream[i - blankingChannelDelayBufferLength];
		}
	}
	
	memcpy(blankingChannelDelayBuffer, originalStream + bufferLength - blankingChannelDelayBufferLength, blankingChannelDelayBufferLength);
}

void VDASIOOutput::DebugSaveBuffersToFile(float *x, float *y, float *z, const char* path) {
	//StringBuilder sb = new StringBuilder();
	//sb.AppendLine("X,Y,Z");
	//for (int i = 0; i < x.BufferSize; i++) {
	//	sb.AppendLine($ "{x[i]:R},{y[i]:R},{z[i]:R}");
	//}
	//File.WriteAllText(path, sb.ToString());
}
