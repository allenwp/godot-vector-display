#include "vector_display.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void VectorDisplay::_bind_methods() {
	ClassDB::bind_method(D_METHOD("start_asio_output"), &VectorDisplay::start_asio_output);
}

VectorDisplay::VectorDisplay() {
	
}

VectorDisplay::~VectorDisplay() {
	if (output) {
		delete output;
		output = nullptr;
	}
}

double value = 0;
void VectorDisplay::_process(double delta) {

}

void VectorDisplay::start_asio_output() {
	if (output) {
		delete output;
	}
	output = new VDASIOOutput();
}