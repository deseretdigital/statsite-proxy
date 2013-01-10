#ifndef PROXY_H
#define PROXY_H
#include "hashring.h"
#include "hashmap.h"

typedef struct {
	hashring *hashring;
	hashmap *conn_map;
} proxy;

/**
 * Creates a new proxy and allocates space for it.
 *
 * @arg proxyptr The value of this pointer will contain the initialized proxy.
 * @arg filename The server-definition file which defines our hashring.
 * @return 0 on success 1 on failure.
 */
int proxy_init(proxy **proxyptr, char* filename);

/**
 * Add connection to server.
 *
 * @arg proxy struct returned by proxy_init.
 * @arg server_key Sever address (ip:port).
 * @arg conn Connection to proxied server
 * @return 1 if the server key is new, 0 if updated.
 */
int proxy_put_conn(proxy *proxy, char *server_key, void *conn);

/**
 * Retrieve connection to server based on key.
 *
 * @arg proxy struct returned by proxy_init.
 * @arg server_key Sever address (ip:port).
 * @arg conn Set to connection of proxied server
 * @return 0 on success. -1 if not found.
 */
int proxy_get_conn(proxy *proxy, char *server_key, void **conn);

/**
 * Retrieve connection to server based on metric key.
 *
 * @arg proxy struct returned by proxy_init.
 * @arg metric_key Sever address (ip:port).
 * @arg conn Set to connection of proxied server..
 * @return 0 on success. -1 if not found.
 */
int proxy_get_route_conn(proxy *proxy, char *metric_key, void **conn);

/**
 * Destroys a proxy and cleans frees all associated memory
 *
 * @arg proxy Pointer to the proxy to be destroy.
 */
void proxy_destroy(proxy *proxy);

#endif
