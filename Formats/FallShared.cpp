#include "FallShared.h"

Options ConvOptions;

PID_t::PID_t(ByteReader* reader)
{
	*reader >> PIDType;
	reader->u8();
	*reader >> PIDNum;
}