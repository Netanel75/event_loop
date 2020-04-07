#ifndef __EXEC_LIB__
#define __EXEC_LIB__

#include <stdbool.h>

#include "mbuf.h"

typedef void (*async_cb_t)(struct mbuf *buffer);
typedef int (*sync_cb_t)(struct mbuf *buffer);

int exec_proccess(char **cmd, bool is_async, sync_cb_t sync_cb);

void init_event_loop(async_cb_t async);

void start_event_loop(unsigned int number_of_dockers);

void destroy_event_loop(void);

#endif