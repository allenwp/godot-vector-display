#ifndef VECTORDISPLAY_H
#define VECTORDISPLAY_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class VectorDisplay : public Node {
	GDCLASS(VectorDisplay, Node)

private:

protected:
	static void _bind_methods();

public:
	VectorDisplay();
	~VectorDisplay();

	void _process(double delta) override;
};

}

#endif