#include "vd_asio_output.h"

using namespace godot; 

VDASIOOutput::VDASIOOutput() {
	asioDrivers = new AsioDrivers();
	driverName = new char[400];
	asioDrivers->getDriverNames(&driverName, 1);
}

VDASIOOutput::~VDASIOOutput() {
}
