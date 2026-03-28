#ifndef _PLAT_H_
#define _PLAT_H_

void plat_net_init(void);
bool plat_net_connect(const char *server_name);
bool plat_net_connected(void);
void plat_net_disconnect(void);
void plat_net_update(void);
void plat_net_send_char(char c);
void plat_net_send_string(unsigned char *text);

#endif // _PLAT_H_

