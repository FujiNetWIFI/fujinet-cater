#include <c64.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "net.h"

#define LFN 2     // The logical file number to use for I/O
#define DEV 16    // The network device #
#define SAN 2     // The secondary address (SA) to use on DEV.
#define CMD 15    // The secondary address for the Command channel

char netstatus_cmd[] = "statusb,2";
bool connected=false;
bool trip=false;
uint8_t tick = 0;
extern void ihsetup();
char url[256];
uint8_t buffer[1024];

uint16_t network_bw;
uint8_t network_conn;
uint8_t network_error;


int16_t get_key_if_available(void);

/*-----------------------------------------------------------------------*/
uint8_t c_network_status(uint16_t *bw, uint8_t *c, uint8_t *err) {
  uint8_t status[4];
  if (cbm_write(CMD, netstatus_cmd, strlen( netstatus_cmd )) != strlen(netstatus_cmd))  {
    return 1;
  }

  if (cbm_read(CMD, status, 4) != 4) {
    return 1;
  }

  if (bw) {
    *bw = (uint16_t)(status[1] << 8) | status[0]; // bytes waiting
  }
  if (c) {
    *c = status[2]; // connected
  }
  if (err) {
    *err = status[3]; // error code.
  }

  return 0;
}


/*-----------------------------------------------------------------------*/
void net_init() {
  ihsetup();
  if( cbm_open( CMD,DEV,CMD, "") ) {
    puts("Error opening command channel");
    quit();
  }
}

/*-----------------------------------------------------------------------*/
bool net_connect(const char *dev){
  // I'm assuming N: is prepended by main and not used by cbm_open() calls
  memcpy((void*) url, dev+2, strlen( dev+2 ) );
  if( cbm_open( LFN, DEV, SAN, url) ) {
    connected = false;
  } else {
    connected = true; 
  }
  return connected;
}

/*-----------------------------------------------------------------------*/
bool net_connected() {
  return connected;
}

/*-----------------------------------------------------------------------*/
void net_disconnect() {
  cbm_close(LFN);
  cbm_close(CMD);
}

/*-----------------------------------------------------------------------*/
void net_update() {
  int16_t retval;
  uint8_t *bufptr;
  if (trip ) {
    retval = cbm_read(LFN,buffer,sizeof(buffer));
    if( retval < 0 ) { 
      // ( see cbm.h :  0 means end-of-file; -1 means error.
      c_network_status( &network_bw, &network_conn, & network_error );
      connected = network_conn;

    } else {
      bufptr = buffer;
      while( retval-- ) {
        vt100_process_inbound_char(*bufptr++);
      }
    }
    trip = false;
  }

  retval = get_key_if_available();
  if (retval >= 0) {
    vt100_process_outbound_char(retval);
  }
}

/*-----------------------------------------------------------------------*/
void net_send_char(char c) {
  // cbm.h  Returns the number of actually-written bytes, or -1 in case of an error;
  if( cbm_write( LFN, &c, 1 ) < 0 ) {
    c_network_status( &network_bw, &network_conn, & network_error );
    connected = network_conn;
  }
}

/*-----------------------------------------------------------------------*/
void net_send_string(unsigned char *s) {
  if( cbm_write( LFN, s, strlen((char*)s)) < 0 ) {
    c_network_status( &network_bw, &network_conn, & network_error );
    connected = network_conn;
  }
}

