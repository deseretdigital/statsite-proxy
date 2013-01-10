#ifndef HASHRING_H
#define HASHRING_H
#include "ketama.h"

typedef struct hashring hashring;

/**
 * Creates a new hashring and allocates space for it.
 *
 * @arg hashringptr The value of this pointer will contain the initialized hashring.
 * @arg filename The server-definition file which defines our hashring.
 * @return 0 on success 1 on failure.
 */
int hashring_init(hashring **hashringptr, char *filename);

/**
 * Selects server based on consistent hash of key.
 *
 * @arg hashring Hashring struct returned by hashring_init.
 * @arg key The key to be hashed.
 * @return The server (ip:port) that key hashes to.
 */
char* hashring_getserver(hashring *hashring, char *key);

/**
 * Retrieves server definition info
 *
 * @arg hashring Hashring struct returned by hashring_init.
 * @return pointer to server info struct
 */
ketama_serverinfo* hashring_getserver_info(hashring *hashring);

/**
 * Retrieves total number of servers in hashring
 *
 * @arg hashring Hashring struct returned by hashring_init.
 * @return number of servers
 */
int hashring_get_numservers(hashring *hashring);

/**
 * Destroys a hashring and frees all associated memory
 *
 * @arg hashring Pointer to the hashring struct to be destroy.
 */
void hashring_destroy(hashring *hashring);


/**
 * Retrieve error message.
 *
 * @return The latest error that occurred.
 */
char* hashring_error();

#endif
