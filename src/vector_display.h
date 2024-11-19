#ifndef VECTORDISPLAY_H
#define VECTORDISPLAY_H

#include <godot_cpp/classes/node.hpp>
#include "output/vd_asio_output.h"

namespace vector_display {

class VectorDisplay : public Node {
	GDCLASS(VectorDisplay, Node)

private:

protected:
	static void _bind_methods();
	VDASIOOutput* output = nullptr;
	int starvedSamples = 0;
	double previousFrameTime;

	enum WriteStateEnum {
		Buffer1,
		Buffer2
	};
	WriteStateEnum WriteState = WriteStateEnum::Buffer1;

	/// <summary>
	/// Used for determining how blanking should behave for the first sample of a new frame.
	/// </summary>
	VDSample previousFinalSample;

	void reset_buffers();

	TypedArray<PackedVector3Array> GetScreenSpaceSamples(TypedArray<PackedVector4Array> &worldSpaceResult);

	/// <param name="previousFrameEndSample">The sample that was drawn right before starting to draw this frame. (Last sample from the previous frame)</param>
	VDSample *CreateFrameBuffer(TypedArray<PackedVector3Array> samples, VDSample previousFrameEndSample, int &blankingSamplesOut, int &wastedSamplesOut, int &bufferLengthOut);

	static float EaseInOutPower(float progress, int power);

public:
	VectorDisplay();
	~VectorDisplay();

	void start_asio_output();

	void _ready() override;
	void _process(double delta) override;

	int get_last_starved_samples();
	int get_previous_frame_time();
};

}

#endif