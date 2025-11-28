#ifndef DEVICES_H
#define DEVICES_H

#include "types.h"

void devices_init();
u8 io_read8(u16 port);
void io_write8(u16 port, u8 val);

#endif
