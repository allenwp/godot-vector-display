#include "vector_display.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void VectorDisplay::_bind_methods() {
}

VectorDisplay::VectorDisplay() {
	output = new VDASIOOutput();
}

VectorDisplay::~VectorDisplay() {
}

void VectorDisplay::_process(double delta) {
	printf(output->driverName);
}