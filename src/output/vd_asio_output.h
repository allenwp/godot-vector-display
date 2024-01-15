#ifndef VD_ASIO_OUTPUT_H
#define VD_ASIO_OUTPUT_H

#include "asio/host/asiodrivers.h"

namespace godot {

class VDASIOOutput {
private:
protected:
	AsioDrivers *asioDrivers = 0;

public:
	VDASIOOutput();
	~VDASIOOutput();
};

} // namespace godot

#endif