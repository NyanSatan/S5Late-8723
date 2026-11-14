#ifndef NAMES_H
#define NAMES_H

#include <stdint.h>
#include <stddef.h>

const uint16_t new_name_desc[] = {0x3 << 8 /* type */ | 0x1E /* len */, 'S', '5', 'L', '8', '7', '4', '0', ' ', 'p', 'w', 'n', 'D', 'F', 'U'};
const size_t  new_name_desc_len = sizeof(new_name_desc);

#endif
