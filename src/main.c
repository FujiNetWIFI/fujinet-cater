#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <cc65.h>
#include <fujinet-network.h>

void vt100_init_terminal(void);
void vt100_exit_terminal(void);
void __fastcall__ vt100_process_inbound_char(uint8_t c);
void __fastcall__ vt100_process_outbound_char(uint8_t c);

extern void vt100_screen_cols;
extern void vt100_screen_rows;
#pragma zpsym ("vt100_screen_cols");
#pragma zpsym ("vt100_screen_rows");

#if defined(__ATARI__)
bool trip=false;       /* Used for PROCEED interrupt. */
uint8_t old_enabled=0; /* Saved interrupt state */
uint8_t old_connected=false; /* Old connected state */
void *old_vprced;      /* Saved PROCEED interrupt vector */
extern void ih();      /* PROCEED interrupt handler   */
#endif /* __ATARI__*/

#if defined(__ATARI__) || defined(__C64__)
int16_t get_key_if_available(void);
#endif

char device[256];
uint8_t buffer[1024];

void vt100_beep(void) {
  putchar('\a');
}

void vt100_quit(void) {
  network_close(device);
}

void __fastcall__ vt100_send_char(uint8_t c) {
  network_write(device, &c, 1);
}

void __fastcall__ vt100_send_string(uint8_t *s) {
  network_write(device, s, strlen((char *)s));
}

/**
 * @brief Set up network interrupts, if available.
 */
void network_interrupt_enable(void)
{
#if defined(__ATARI__)
  old_vprced  = OS.vprced;         /* Store old VPRCED vector for exit  */
  old_enabled = PIA.pactl & 1;     /* Keep track of old interrupt state */
  PIA.pactl   &= (~1);             /* Turn off interrupts momentarily.  */
  OS.vprced   = ih;                /* Set interrupt handler.            */
  PIA.pactl   |= 1;                /* Tell PIA we are ready.            */
#endif /* __ATARI__*/
}

/**
 * @brief Tear down interrupt when we're exiting.
 */
void network_interrupt_disable(void)
{
#if defined(__ATARI__)
  PIA.pactl &= (~1); /* Disable interrupts. */
  OS.vprced  = old_vprced;      /* Bring back old PROCEED vector */
  PIA.pactl |= old_enabled;    /* Bring back original PACTL state. */
#endif /* __ATARI__ */
}

static bool connected(void)
{
  uint16_t bw;
  uint8_t c;
  uint8_t err;

#if defined(__ATARI__)
  if (!trip)
    return old_connected;
#endif /* __ATARI__ */
  
  network_status(device, &bw, &c, &err);

#if defined(__ATARI__)
  old_connected = c;
#endif /* __ATARI__ */

  return c;
}

static void quit(void)
{
  if (doesclrscrafterexit()) {
    puts("Press any key to continue ...");
    cgetc();
  }
  exit(EXIT_FAILURE);
}

static void readline(char *s)
{
    uint16_t i = 0;
    char c;

    cursor(1);

    do {
      c = cgetc();

      if (isprint(c)) {
        putchar(c);
        s[i++] = c;
      }
      else if ((c == CH_CURS_LEFT) || (c == CH_DEL)) {
        if (i) {
          putchar(CH_CURS_LEFT);
          putchar(' ');
          putchar(CH_CURS_LEFT);
          --i;
        }
      }
    } while (c != CH_ENTER);
    putchar('\n');
    s[i] = '\0';

    cursor(0);
}

void main(int argc, char *argv[])
{
  uint8_t cols = (uint8_t)&vt100_screen_cols;
  uint8_t rows = (uint8_t)&vt100_screen_rows;
  register int16_t retval;
  register uint8_t *bufptr;

  if (doesclrscrafterexit()) {
    clrscr();
  }

  if (network_init() != FN_ERR_OK) {
    puts("Network init error");
    quit();
  }

  strcpy(device, "N:");
  if (argc == 2) {
    strcpy(device + 2, argv[1]);
  } else {
    puts("URL examples:");
    puts("    telnet://host");
    puts("    ssh://user:password@host\n");
    puts("Enter Telnet or SSH URL:");
    readline(device + 2);
  }

  if (network_open(device, OPEN_MODE_RW, OPEN_TRANS_NONE) != FN_ERR_OK) {
    fputs("Network open error: ", stdout);
    puts(device + 2);
    quit();
  }

  /* Only enable interrupts when we are successfully connected. */
  network_interrupt_enable();
  
  vt100_init_terminal();

  while (connected()) {

#if defined(__ATARI__)
    if (!trip)
      goto read_atari_keyboard;
#endif /* __ATARI__ */
    
    retval = network_read_nb(device, buffer, sizeof(buffer));
    if (retval > 0) {
      bufptr = buffer;
      while (retval--) {
        vt100_process_inbound_char(*bufptr++);
      }
    }
    
#if defined(__ATARI__) || defined(__C64__)
  read_atari_keyboard:
    retval = get_key_if_available();
    if (retval >= 0) {
      vt100_process_outbound_char(retval);
    }

    trip = 0;       /* Clear the trip wire.                    */
    PIA.pactl |= 1; /* Interrupt serviced, ready for next one. */
    
#else
    if (kbhit()) {
      vt100_process_outbound_char(cgetc());
    }
#endif
  }

  /* Disable interrupts */
  network_interrupt_disable();
  
  vt100_exit_terminal();

  puts("Connection closed.");
}
