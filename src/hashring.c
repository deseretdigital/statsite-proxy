#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "hashring.h"

struct hashring {
	ketama_serverinfo serverinfo;
	ketama_continuum continuum;
};

/**
 * Creates a new hashring and allocates space for it.
 * @arg hashringptr The value of this pointer will contain the initialized hashring.
 * @arg filename The server-definition file which defines our hashring.
 * @return 0 on success 1 on failure.
 */
int hashring_init(hashring **hashringptr, char* filename) {

	// Allocate the hashring
	*hashringptr = calloc(1, sizeof(hashring));

	// Initialize continuum
	int ketama_res = ketama_roll( &(*hashringptr)->continuum, filename, &(*hashringptr)->serverinfo );
	if (ketama_res == 0) {
		return 1;
	}

#ifdef DEBUG
    ketama_print_continuum((*hashringptr)->continuum);
#endif

	return 0;
}

/**
 * Selects server based on consistent hash of key.
 * @arg hashringptr Hashring struct returned by hashring_init.
 * @arg key The key to be hashed.
 * @return The server (ip:port) that key hashes to.
 */
const char* hashring_getserver(hashring *hashringptr, char *key) {

	mcs* server = ketama_get_server(key, hashringptr->continuum);

	return server->ip;
}

/**
 * Retrieves server definition info
 * @arg hashring Hashring struct returned by hashring_init.
 * @return pointer to server info struct
 */
ketama_serverinfo* hashring_getserver_info(hashring *hashring) {
	return &hashring->serverinfo;
}

/**
 * Destroys a hashring and cleans up all associated memory
 * @arg hashringptr Pointer to the hashring to be destroy. Frees memory.
 */
void hashring_destroy(hashring *hashringptr) {

	free(hashringptr->serverinfo.serverinfo);
    ketama_smoke(hashringptr->continuum);

    free(hashringptr);
}

/**
 * Retrieve error message.
 * @return The latest error that occurred.
 */
char* hashring_error() {
	return ketama_error();
}



