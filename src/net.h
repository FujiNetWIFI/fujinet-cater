#ifndef _PLAT_H_
#define _PLAT_H_

void net_init(void);
bool net_connect(const char *server_name);
bool net_connected(void);
void net_disconnect(void);
void net_update(void);
void net_send_char(char c);
void net_send_string(unsigned char *text);

#endif // _PLAT_H_

