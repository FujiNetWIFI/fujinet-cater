#include <atari.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "plat.h"

#include "fujinet-network.h"

#include <peekpoke.h>

uint8_t device[256];
uint8_t buffer[1024];

#define CH      0x02FC
#define CH1     0x02F2

bool trip=false;
uint8_t tick = 0;

int16_t get_key_if_available(void);


/*-----------------------------------------------------------------------*/
void plat_net_init() {
  if (network_init() != FN_ERR_OK) {
    puts("Network init error");
    quit();
  }
  (*(uint8_t*)0x41) = 0; // turn off SIO beeps
}

/*-----------------------------------------------------------------------*/
bool plat_net_connect(const char *server){
  memcpy((void*) device, server, strlen( server ) );
  if (network_open((char*)device, OPEN_MODE_RW, OPEN_TRANS_NONE) != FN_ERR_OK) {
    return false;   
  }    
  return true;   
}

/*-----------------------------------------------------------------------*/
bool plat_net_connected() {
  uint16_t bw;
  uint8_t c;
  uint8_t err;
  network_status((char*)device, &bw, &c, &err);
  return c;
}

/*-----------------------------------------------------------------------*/
void plat_net_disconnect() {
  network_close( (char*)device );
}

/*-----------------------------------------------------------------------*/
void plat_net_update() {
  int16_t retval;
  uint8_t *bufptr; 

  tick++;
  if( tick%2 == 0 ) {
    tick = 0;
  }

  retval = network_read_nb((char*)device, buffer, sizeof(buffer));
  if (retval > 0) {
    bufptr = buffer;
    while (retval--) {
      vt100_process_inbound_char(*bufptr++);
    }
  }

  retval = get_key_if_available();
  POKE(CH, 0xFF);      // reset key
  POKE(CH1, 0xFF);
  if (retval >= 0) {
    vt100_process_outbound_char(retval);
  }
}

/*-----------------------------------------------------------------------*/
void plat_net_send_char(char c) {
  network_write((char*)device, (unsigned char*)&c, 1);
}

/*-----------------------------------------------------------------------*/
void plat_net_send_string(unsigned char *s) {
  network_write((char*)device, s, strlen((char *)s));
}

