#ifndef VECTORDISPLAY_H
#define VECTORDISPLAY_H

#include <godot_cpp/classes/node.hpp>
#include "output/vd_asio_output.h"

namespace godot {

class VectorDisplay : public Node {
	GDCLASS(VectorDisplay, Node)

private:

protected:
	static void _bind_methods();
	VDASIOOutput* output;

public:
	VectorDisplay();
	~VectorDisplay();

	void _process(double delta) override;
};

}

#endif