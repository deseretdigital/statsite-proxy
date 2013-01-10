#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include "proxy.h"


/**
 * Creates a new proxy and allocates space for it.
 *
 * @arg proxyptr The value of this pointer will contain the initialized proxy.
 * @arg filename The server-definition file which defines our hashring.
 * @return 0 on success 1 on failure.
 */
int proxy_init(proxy **proxyptr, char* filename) {

	// Allocate space for proxy_connections struct
	*proxyptr = calloc(1, sizeof(proxy));

	// Initialize hashring
	int hashring_res = hashring_init(&(*proxyptr)->hashring, filename);

	if (hashring_res == 1) {
		syslog(LOG_ERR, "%s", hashring_error());
		return 1;
	}

	// Initialize hashmap
	int numservers = hashring_get_numservers((*proxyptr)->hashring);
	int hashmap_res = hashmap_init(numservers+1, &(*proxyptr)->conn_map);
	if (hashmap_res != 0) {
		syslog(LOG_ERR, "Failed to initialize connection map.");
		return 1;
	}

	return 0;
}

/**
 * Add connection to server.
 *
 * @arg proxy struct returned by proxy_init.
 * @arg server_key Sever address (ip:port).
 * @arg conn Connection to proxied server
 * @return 1 if the server key is new, 0 if updated.
 */
int proxy_put_conn(proxy *proxy, char *server_key, void *conn) {
	return hashmap_put(proxy->conn_map, server_key, conn);
}

/**
 * Retrieve connection to server based on key.
 *
 * @arg proxy struct returned by proxy_init.
 * @arg server_key Sever address (ip:port).
 * @arg conn Set to connection of proxied server
 * @return 0 on success. -1 if not found.
 */
int proxy_get_conn(proxy *proxy, char *server_key, void **conn) {
	return hashmap_get(proxy->conn_map, server_key, conn);
}

/**
 * Retrieve connection to server based on metric key.
 *
 * @arg proxy struct returned by proxy_init.
 * @arg metric_key Sever address (ip:port).
 * @arg conn Set to connection of proxied server..
 * @return 0 on success. -1 if not found.
 */
int proxy_get_route_conn(proxy *proxy, char *metric_key, void **conn) {
	return proxy_get_conn(proxy, hashring_getserver(proxy->hashring, metric_key), conn);
}

/**
 * Destroys a proxy and cleans frees all associated memory
 *
 * @arg proxy Pointer to the proxy to be destroy.
 */
void proxy_destroy(proxy *proxy) {
	hashring_destroy(proxy->hashring);
	hashmap_destroy(proxy->conn_map);
	free(proxy);
}


