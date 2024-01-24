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

	/// <summary>
	/// Used for determining how blanking should behave for the first sample of a new frame.
	/// </summary>
	VDSample previousFinalSample;

	TypedArray<Array> GetScreenSpaceSamples();

	/// <param name="previousFrameEndSample">The sample that was drawn right before starting to draw this frame. (Last sample from the previous frame)</param>
	VDSample *CreateFrameBuffer(TypedArray<Array> samples, VDSample previousFrameEndSample, int &blankingSamplesOut, int &wastedSamplesOut, int &bufferLengthOut);

	static float EaseInOutPower(float progress, int power);

public:
	VectorDisplay();
	~VectorDisplay();

	enum WriteStateEnum {
		Buffer1,
		Buffer2
	};
	WriteStateEnum WriteState = WriteStateEnum::Buffer1;

	void start_asio_output();
	void _process(double delta) override;
};

}

#endif