#ifndef VD_ASIO_OUTPUT_H
#define VD_ASIO_OUTPUT_H

#include "asio/common/asiosys.h"
#include "asio/common/asio.h"
#include "asio/host/asiodrivers.h"

namespace godot {

class VDASIOOutput {
private:
protected:
	enum {
		// number of input and outputs supported by the host application
		// you can change these to higher or lower values
		kMaxInputChannels = 0,
		kMaxOutputChannels = 4
	};

	typedef struct DriverInfo {
		// ASIOInit()
		ASIODriverInfo driverInfo;

		// ASIOGetChannels()
		long inputChannels;
		long outputChannels;

		// ASIOGetBufferSize()
		long minSize;
		long maxSize;
		long preferredSize;
		long granularity;

		// ASIOGetSampleRate()
		ASIOSampleRate sampleRate;

		// ASIOOutputReady()
		bool postOutput;

		// ASIOGetLatencies ()
		long inputLatency;
		long outputLatency;

		// ASIOCreateBuffers ()
		long inputBuffers; // becomes number of actual created input buffers
		long outputBuffers; // becomes number of actual created output buffers
		ASIOBufferInfo bufferInfos[kMaxInputChannels + kMaxOutputChannels]; // buffer info's

		// ASIOGetChannelInfo()
		ASIOChannelInfo channelInfos[kMaxInputChannels + kMaxOutputChannels]; // channel info's
		// The above two arrays share the same indexing, as the data in them are linked together

		// Information from ASIOGetSamplePosition()
		// data is converted to double floats for easier use, however 64 bit integer can be used, too
		double nanoSeconds;
		double samples;
		double tcSamples; // time code samples

		// bufferSwitchTimeInfo()
		ASIOTime tInfo; // time info state
		unsigned long sysRefTime; // system reference time, when bufferSwitch() was called

		// Signal the end of processing in this example
		bool stopped;
	} DriverInfo;

	static VDASIOOutput *_static_instance;
	static ASIOTime *bufferSwitchTimeInfo(ASIOTime *timeInfo, long index, ASIOBool processNow);
	static void bufferSwitch(long index, ASIOBool processNow);
	static void sampleRateChanged(ASIOSampleRate sRate);
	static long asioMessages(long selector, long value, void *message, double *opt);
	static unsigned long get_sys_reference_time();

	DriverInfo asioDriverInfo = { 0 };
	ASIOCallbacks asioCallbacks;

	bool _need_to_stop = false;
	bool _need_to_dispose_buffers = false;
	bool _need_to_exit = false;
	bool _need_to_remove_current_driver = false;

	long init_asio_static_data(DriverInfo *asioDriverInfo);
	ASIOError create_asio_buffers(DriverInfo *asioDriverInfo);

	void _cleanup();

public:
	VDASIOOutput();
	~VDASIOOutput();
};

} // namespace godot

#endif