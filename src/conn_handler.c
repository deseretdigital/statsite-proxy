#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <netinet/in.h>
#include "conn_handler.h"

/*
 * Binary defines
 */
#define BIN_TYPE_KV      0x1
#define BIN_TYPE_COUNTER 0x2
#define BIN_TYPE_TIMER   0x3

#define BIN_OUT_NO_TYPE 0x0
#define BIN_OUT_SUM     0x1
#define BIN_OUT_SUM_SQ  0x2
#define BIN_OUT_MEAN    0x3
#define BIN_OUT_COUNT   0x4
#define BIN_OUT_STDDEV  0x5
#define BIN_OUT_MIN     0x6
#define BIN_OUT_MAX     0x7
#define BIN_OUT_PCT     0x80

/* Static method declarations */
static int handle_binary_client_connect(statsite_proxy_conn_handler *handle);
static int handle_ascii_client_connect(statsite_proxy_conn_handler *handle);
static int buffer_after_terminator(char *buf, int buf_len, char terminator, char **after_term, int *after_len);

// This is the magic byte that indicates we are handling
// a binary command, instead of an ASCII command. We use
// 170 (0xaa) value.
static unsigned char BINARY_MAGIC_BYTE = 0xaa;
static int BINARY_HEADER_SIZE = 12;

static statsite_proxy_config *GLOBAL_CONFIG;

/**
 * Invoked to initialize the conn handler layer.
 */
void init_conn_handler(statsite_proxy_config *config) {
    // Store the config
    GLOBAL_CONFIG = config;
}

/**
 * Invoked by the networking layer when there is new
 * data to be handled. The connection handler should
 * consume all the input possible, and generate responses
 * to all requests.
 * @arg handle The connection related information
 * @return 0 on success.
 */
int handle_client_connect(statsite_proxy_conn_handler *handle) {
    // Try to read the magic character, bail if no data
    unsigned char magic = 0;
    if (peek_client_bytes(handle->conn, 1, &magic) == -1) return 0;

    // Check the magic byte
    if (magic == BINARY_MAGIC_BYTE)
        return handle_binary_client_connect(handle);
    else
        return handle_ascii_client_connect(handle);
}


/**
 * Invoked to handle ASCII commands. This is the default
 * mode for statsite-proxy, to be backwards compatible with statsd
 * @arg handle The connection related information
 * @return 0 on success.
 */
static int handle_ascii_client_connect(statsite_proxy_conn_handler *handle) {
    // Look for the next command line
    char *buf, *val_str, *type_str;
    metric_type type;
    int buf_len, should_free, status, after_len;
    while (1) {
        status = extract_to_terminator(handle->conn, '\n', &buf, &buf_len, &should_free);
        if (status == -1) return 0; // Return if no command is available

        // Check for a valid metric
        // Scan for the colon
        status = buffer_after_terminator(buf, buf_len, ':', &val_str, &after_len);
        if (!status) status |= buffer_after_terminator(val_str, after_len, '|', &type_str, &after_len);
        if (status == 0) {
            // Convert the type
            switch (*type_str) {
                case 'c':
                    type = COUNTER;
                    break;
                case 'm':
                    type = TIMER;
                    break;
                case 'k':
                case 'g':
                    type = KEY_VAL;
                    break;
                default:
                    type = UNKNOWN;
            }

            // Apply consistent hashing
            mcs* server = ketama_get_server(buf, handle->hashring);


            // Forward metric


        } else {
            syslog(LOG_WARNING, "Failed parse metric! Input: %s", buf);
        }

        // Make sure to free the command buffer if we need to
        if (should_free) free(buf);
    }

    return 0;
}


/**
 * Invoked to handle binary commands.
 * @arg handle The connection related information
 * @return 0 on success.
 */
static int handle_binary_client_connect(statsite_proxy_conn_handler *handle) {
    metric_type type;
    int status, should_free;
    char *key;
    unsigned char header[BINARY_HEADER_SIZE];
    uint8_t type_input;
    uint16_t key_len;
    while (1) {
        // Peek and check for the header. This is 12 bytes.
        // Magic byte - 1 byte
        // Metric type - 1 byte
        // Key length - 2 bytes
        // Metric value - 8 bytes
        status = peek_client_bytes(handle->conn, BINARY_HEADER_SIZE, (char*)&header);
        if (status == -1) return 0; // Return if no command is available

        // Check for the magic byte
        if (header[0] != BINARY_MAGIC_BYTE) {
            syslog(LOG_WARNING, "Received command from binary stream without magic byte!");
            return -1;
        }

        // Get the metric type
        type_input = header[1];
        switch (type_input) {
            case BIN_TYPE_KV:
                type = KEY_VAL;
                break;
            case BIN_TYPE_COUNTER:
                type = COUNTER;
                break;
            case BIN_TYPE_TIMER:
                type = TIMER;
                break;
            default:
                type = UNKNOWN;
                syslog(LOG_WARNING, "Received command from binary stream with unknown type: %u!", type_input);
                break;
        }

        // Extract the key length and value
        memcpy(&key_len, &header[2], 2);

        // Abort if we haven't received the full key, wait for the data
        if (available_bytes(handle->conn) < BINARY_HEADER_SIZE + key_len)
            return 0;

        // Seek past the header
        seek_client_bytes(handle->conn, BINARY_HEADER_SIZE);

        // Read the key now
        read_client_bytes(handle->conn, key_len, &key, &should_free);

        // Verify the key contains a null terminator
        if (memchr(key, '\0', key_len) == NULL) {
            syslog(LOG_WARNING, "Received command from binary stream with non-null terminated key: %.*s!", key_len, key);

            // For safety, we will just set the last byte to be null, and continue processing
            *(key + key_len - 1) = 0;
        }

        // Apply consistent hashing
        mcs* server = ketama_get_server(key, handle->hashring);


        // Make sure to free the command buffer if we need to
        if (should_free) free(key);
    }

    return 0;
}


/**
 * Scans the input buffer of a given length up to a terminator.
 * Then sets the start of the buffer after the terminator including
 * the length of the after buffer.
 * @arg buf The input buffer
 * @arg buf_len The length of the input buffer
 * @arg terminator The terminator to scan to. Replaced with the null terminator.
 * @arg after_term Output. Set to the byte after the terminator.
 * @arg after_len Output. Set to the length of the output buffer.
 * @return 0 if terminator found. -1 otherwise.
 */
static int buffer_after_terminator(char *buf, int buf_len, char terminator, char **after_term, int *after_len) {
    // Scan for a space
    char *term_addr = memchr(buf, terminator, buf_len);
    if (!term_addr) {
        *after_term = NULL;
        return -1;
    }

    // Convert the space to a null-seperator
    *term_addr = '\0';

    // Provide the arg buffer, and arg_len
    *after_term = term_addr+1;
    *after_len = buf_len - (term_addr - buf + 1);
    return 0;
}

