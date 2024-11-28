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
	double debug_process_time = 0;
	int64_t debug_process_timestamp = 0;
	int blankingSampleCount = 0;
	int wastedSampleCount = 0;
	double debug_render_time = 0;
	int debug_samples_3d_count = 0;
	bool debug_calibration_enabled = false;

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

	TypedArray<PackedVector3Array> RenderScreenSpaceSamples(TypedArray<PackedVector4Array> &worldSpaceResult);

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
	double debug_get_process_time();
	void reset_asio_profiling();
	double get_asio_min_time_between_buffer_switch();
	double get_asio_max_time_between_buffer_switch();
	double get_asio_min_time_to_copy_buffers();
	double get_asio_max_time_to_copy_buffers();
	double debug_get_asio_min_headroom();
	double debug_get_asio_max_headroom();
	int debug_get_blanking_sample_count();
	int debug_get_wasted_sample_count();
	void debug_set_calibration_enabled(bool value);
	double debug_get_render_time();
	int debug_get_samples_3d_count();
	bool debug_get_calibration_enabled();
	void debug_set_float_property(double value);
	double debug_get_float_property();
};

}

#endif