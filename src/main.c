#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <cc65.h>
#include "net.h"

void vt100_init_terminal(void);
void vt100_exit_terminal(void);

extern void vt100_screen_cols;
extern void vt100_screen_rows;
#pragma zpsym ("vt100_screen_cols");
#pragma zpsym ("vt100_screen_rows");

char uri[256];

void vt100_beep(void) {
  putchar('\a');
}

void vt100_quit(void) {
  net_disconnect();
}

void __fastcall__ vt100_send_char(uint8_t c) {
  net_send_char( c );
}

void __fastcall__ vt100_send_string(uint8_t *s) {
  net_send_string( s );
}

void quit(void)
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

  if (doesclrscrafterexit()) {
    clrscr();
  }

  // get networking started.
  net_init();

  // get protocol and destination from user
  strcpy(uri, "N:");
  if (argc == 2) {
    strcpy(uri + 2, argv[1]);
  } else {
    puts("URL examples:");
    puts("    telnet://host");
    puts("    ssh://user:password@host\n");
    puts("Enter Telnet or SSH URL:");
    readline(uri + 2);
  }

  if( ! net_connect( uri )  ) {
    fputs("Network open error: ", stdout);
    puts(uri + 2);
    quit();
  }

  vt100_init_terminal();

  while (net_connected()) {
    net_update();
  }

  vt100_exit_terminal();

  puts("Connection closed.");
}
