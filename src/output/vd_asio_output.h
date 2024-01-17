#ifndef VD_ASIO_OUTPUT_H
#define VD_ASIO_OUTPUT_H

#include "asio/common/asiosys.h"
#include "asio/common/asio.h"
#include "asio/host/asiodrivers.h"

namespace godot {

class VDASIOOutput {
private:
protected:
	AsioDrivers *asioDrivers = 0;

public:
	char *driverName;
	VDASIOOutput();
	~VDASIOOutput();
};

} // namespace godot

#endif