#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdint.h>
#include <stdbool.h>

void __fastcall__ vt100_process_inbound_char(uint8_t c);
void __fastcall__ vt100_process_outbound_char(uint8_t c);
void quit(void);


#endif // _COMMON_H_

