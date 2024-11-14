/* Copyright (C) 2022 Krzysztof Wolski
 * See COPYING for license details */

/*  interface module
    
    This module shows if network interface have ip

    Configuration:
      .str - interface name
      .icon - icon to show when interface have ip address

*/

#include "../kwstatus.h"
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

void
interface(void* self) {
  struct modules* mod = (struct modules*)self;
  struct sockaddr_nl sa = {0};
  struct ifreq ifr;
  struct in_addr addr;
  int watch, ip, err;
  
  // hacky way to control icon
  const char* icon;
  if(mod->icon != NULL) {
    icon = mod->icon;
    mod->icon = "";
  } else {
    puts("interface: icon for this module must be set");
    return;
  }

  sa.nl_family = AF_NETLINK;
  sa.nl_groups = RTMGRP_IPV4_IFADDR;

  watch = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if(bind(watch, (struct sockaddr *) &sa, sizeof(sa)) != 0) {
    fputs("interface: cannot bind netlink", stderr);
    return;
  }
    

  ip = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name, mod->str, IFNAMSIZ);
  

  while(1) {
    err = ioctl(ip, SIOCGIFADDR, &ifr);
    addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;

    // check for link-local and 0.0.0.0
    if(err == 0 && (addr.s_addr & 0x0000ffff) != 0x0000fea9 && addr.s_addr != 0) {
      printf("%x\n", addr.s_addr);
      mod_update(mod, icon);
    } else {
      mod_update(mod, "");
    }

    memset(&ifr.ifr_addr, 0, sizeof(struct sockaddr)); // reset address
    recv(watch, NULL, 0, 0);
  }
}
