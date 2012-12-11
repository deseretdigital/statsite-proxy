#ifndef CONN_HANDLER_H
#define CONN_HANDLER_H
#include "config.h"
#include "networking.h"
#include "ketama.h"

/**
 * This structure is used to communicate
 * between the connection handlers and the
 * networking layer.
 */
typedef struct {
    statsite_proxy_config *config;     // Global configuration
    ketama_continuum hashring;         // Global hashring
    statsite_proxy_conn_info *conn;    // Opaque handle into the networking stack
} statsite_proxy_conn_handler;

typedef enum {
    UNKNOWN,
    KEY_VAL,
    COUNTER,
    TIMER
} metric_type;

/**
 * Invoked to initialize the conn handler layer.
 */
void init_conn_handler(statsite_proxy_config *config);

/**
 * Invoked by the networking layer when there is new
 * data to be handled. The connection handler should
 * consume all the input possible, and generate responses
 * to all requests.
 * @arg handle The connection related information
 * @return 0 on success.
 */
int handle_client_connect(statsite_proxy_conn_handler *handle);

#endif
