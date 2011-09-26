
#ifndef CRAFTD_SURVIVALPROXY_H
#define CRAFTD_SURVIVALPROXY_H

typedef struct _CDSurvivalProxyData {
  uint16_t port;
  const char* hostname;
  struct evdns_base* dnsBase;
} CDSurvivalProxyData;

#endif