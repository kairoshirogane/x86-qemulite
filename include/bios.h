#ifndef BIOS_H
#define BIOS_H

#include "types.h"

void bios_init();
void bios_handle_int(u8 int_num);

#endif
