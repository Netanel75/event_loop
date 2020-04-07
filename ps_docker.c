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
        buffer->buf[buffer->len -1] = 0;
        printf("%s\n", buffer->buf);
    }
    return 0;
}

static int proccess_async(struct mbuf *buffer)
{
    if (strncmp(buffer->buf, "UID", sizeof("UID") - 1)) {
        printf("error couldn't get container data\n");
        return -1;
    }
    buffer->buf[buffer->len -1] = '\0'; //TODO remove shutsup valgrind
    printf("%s\n", buffer->buf);
    return 0;
}

int main()
{
    char *docker_list_args[] = {"docker", "container", "ls", NULL};
    char *docker_ps_args[] = {"docker", "container", "top", "be4bcca0fc08", NULL};

    init_event_loop();
    exec_proccess(docker_list_args, false, ps_extract_ids);
    exec_proccess(docker_ps_args, true, proccess_async);
    start_event_loop();
    destroy_event_loop();

    return 0;
}