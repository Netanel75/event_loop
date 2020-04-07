#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <ctype.h>


#include "mbuf.h"
#include "exec_lib.h"

#define MAX_DOCKERS 256
#define MAX_LINE_SIZE 1024

static int ps_extract_ids(struct mbuf *buffer)
{
    if (buffer && buffer->buf && buffer->len > 0) {
        printf("%s\n", buffer->buf);
    }
    return 0;
}

static void proccess_async(struct mbuf *buffer)
{
    if (strncmp(buffer->buf, "UID", sizeof("UID") - 1)) {
        printf("error couldn't get container data\n");
        return;
    }
    buffer->buf[buffer->len -1] = '\0'; //TODO remove shutsup valgrind
    printf("%s\n", buffer->buf);
}

int main()
{
    char *docker_list_args[] = {"docker", "container", "ls", NULL};
    char *docker_ps_args[] = {"docker", "container", "top", NULL, NULL};

    init_event_loop(proccess_async);
    exec_proccess(docker_list_args, false, ps_extract_ids);
    exec_proccess(docker_ps_args, true, NULL);
    start_event_loop(10);
    destroy_event_loop();

    return 0;
}