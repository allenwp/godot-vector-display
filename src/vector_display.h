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
	VDASIOOutput *output = nullptr;
	unsigned int thisFrameStarvedSamples = 0;
	unsigned int totalStarvedSamples = 0;
	double previousFrameTime = 0;
	double previousHeadroom = 0;

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

	bool is_output_running();

	double get_previous_frame_time();

	int get_last_starved_samples();
	double get_previous_frame_headroom();
	void reset_asio_profiling();
	double get_asio_min_time_between_buffer_switch();
	double get_asio_max_time_between_buffer_switch();
	double get_asio_min_time_to_copy_buffers();
	double get_asio_max_time_to_copy_buffers();
};

}

#endif