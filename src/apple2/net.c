#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "net.h"

#include "fujinet-network.h"

uint8_t device[256];
uint8_t buffer[1024];

/*-----------------------------------------------------------------------*/
void net_init() {
  if (network_init() != FN_ERR_OK) {
    puts("Network init error");
    quit();
  }
}

/*-----------------------------------------------------------------------*/
bool net_connect(const char *server){
  memcpy( (void*)device, server, strlen( server ) );
  if (network_open((char*)device, OPEN_MODE_RW, OPEN_TRANS_NONE) != FN_ERR_OK) {
    return false;
  }
  return true;
}

/*-----------------------------------------------------------------------*/
bool net_connected() {
  uint16_t bw;
  uint8_t c;
  uint8_t err;
  network_status((char*)device, &bw, &c, &err);
  return c;
}

/*-----------------------------------------------------------------------*/
void net_disconnect() {
  network_close( (char*)device );
}

/*-----------------------------------------------------------------------*/
void net_update() {
  int16_t retval;
  uint8_t *bufptr; 
  retval = network_read_nb((char*)device, buffer, sizeof(buffer));
  if (retval > 0) {
    bufptr = buffer;
    while (retval--) {
      vt100_process_inbound_char(*bufptr++);
    }
  }
  if (kbhit()) {
    vt100_process_outbound_char(cgetc());
  }
}

/*-----------------------------------------------------------------------*/
void net_send_char(char c) {
  network_write((char*)device, (unsigned char*)&c, 1);
}

/*-----------------------------------------------------------------------*/
void net_send_string(unsigned char *s) {
  network_write((char*)device, s, strlen((char*)s));
}

