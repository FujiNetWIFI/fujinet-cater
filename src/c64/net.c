#include <c64.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "net.h"

#include "fujinet-network.h"

char device[256];
uint8_t buffer[1024];
bool connected=false;

int16_t get_key_if_available(void);

/*-----------------------------------------------------------------------*/
void net_init() {
  if (network_init() != FN_ERR_OK) {
    puts("Network init error");
    quit();
  }
}

/*-----------------------------------------------------------------------*/
bool net_connect(const char *server){
  uint16_t bw;
  uint8_t c;
  uint8_t err;

  strcpy(device, server);
  if (network_open(device, OPEN_MODE_RW, OPEN_TRANS_NONE) != FN_ERR_OK) {
    return false;
  }
  // IEC doesn't report a failed open, so ask whether it connected
  if (network_status(device, &bw, &c, &err) != FN_ERR_OK || (!c && !bw)) {
    return false;
  }
  connected = true;
  return connected;
}

/*-----------------------------------------------------------------------*/
bool net_connected() {
  return connected;
}

/*-----------------------------------------------------------------------*/
void net_disconnect() {
  network_close(device);
  connected = false;
}

/*-----------------------------------------------------------------------*/
void net_update() {
  uint16_t bw;
  uint8_t c;
  uint8_t err;
  int16_t retval;
  uint8_t *bufptr;

  // network_read_nb() waits for data, so only read what is already waiting
  if (network_status(device, &bw, &c, &err) == FN_ERR_OK) {
    if (bw) {
      retval = network_read_nb(device, buffer, bw < sizeof(buffer) ? bw : sizeof(buffer));
      if (retval > 0) {
        bufptr = buffer;
        while (retval--) {
          vt100_process_inbound_char(*bufptr++);
        }
      }
    } else if (!c) {
      connected = false;
    }
  }

  retval = get_key_if_available();
  if (retval >= 0) {
    vt100_process_outbound_char(retval);
  }
}

/*-----------------------------------------------------------------------*/
void net_send_char(char c) {
  network_write(device, (unsigned char*)&c, 1);
}

/*-----------------------------------------------------------------------*/
void net_send_string(unsigned char *s) {
  network_write(device, s, strlen((char*)s));
}
