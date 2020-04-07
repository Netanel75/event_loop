#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>

#include "exec_lib.h"

#define MAX_EVENTS 5

static int epoll_fd;
static unsigned int proccess_num = true;

struct event_cb {
    sync_cb_t proccess_async;
    int fd;
};

static int handle_epoll_read(int fd, sync_cb_t cb)
{
    struct mbuf buffer;
    char read_buffer[1024];
    int bytes_read;
    int return_value;

    mbuf_init(&buffer, 0);

    do
    {
        bytes_read = read(fd, &read_buffer, sizeof(read_buffer));
        if (bytes_read < 0) {
            return -1;
        }

        if (bytes_read == 0) {
            printf("forked finished\n");
            close(fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
        } else {
            if (mbuf_append(&buffer, read_buffer, bytes_read) == 0) {
                printf("error no memory");
            }
        }
    } while (bytes_read);

    return_value = cb(&buffer);

    mbuf_free(&buffer);
    proccess_num--;

    return return_value;
}

static int exec_proc_sync(int fd, sync_cb_t sync_cb)
{
    int read_bytes;
    static char read_buffer[2048];
    struct mbuf buffer;
    int ret;

    mbuf_init(&buffer, 0);

    do {
        read_bytes = read(fd, read_buffer, sizeof(read_buffer) - 1);
        read_buffer[read_bytes] = '\0';//TODO remove shutsup valgrind
        if (read_bytes < 0) {
            perror("read operaion failed");
        } else if (read_bytes > 0) {
            mbuf_append(&buffer, read_buffer, read_bytes);
        }
    } while (read_bytes > 0);

    close(fd);
    wait(NULL);
    ret = sync_cb(&buffer);
    mbuf_free(&buffer);
    return ret;
}

static void exec_proc_async(int fd, sync_cb_t sync_cb)
{
    struct epoll_event event;

    struct event_cb *event_cb = malloc(sizeof(*event_cb));
    if (!event_cb) {
        printf("no memory\n");
        return;
    }

    event_cb->proccess_async = sync_cb;
    event_cb->fd = fd;

    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.ptr = event_cb;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
        perror("couldn't add event to epoll");
    }
}

int exec_proccess(char **cmd, bool is_async, sync_cb_t sync_cb)
{
    int fork_pid;
    int stdout_pipe[2];

    if (pipe(stdout_pipe)) {
        perror("couldn't create pipe");
        return -1;
    }

    fork_pid = fork();

    if (fork_pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (fork_pid == 0) {
        close(stdout_pipe[0]);
        close(STDOUT_FILENO);

        //redirect stdout
        if (dup2(stdout_pipe[1], STDOUT_FILENO) < 0) {
            perror("couldn't redirect stdout to pipe");
            return -1;
        }

        //redirect stderr
        close(STDERR_FILENO);
        if (dup2(stdout_pipe[1], STDERR_FILENO) < 0) {
            perror("couldn't redirect stderr to pipe");
            return -1;
        }

        execvp(cmd[0], cmd);
        close(stdout_pipe[1]);

    } else {
        close(stdout_pipe[1]);
        if (is_async) {
            exec_proc_async(stdout_pipe[0], sync_cb);
        } else {
            return exec_proc_sync(stdout_pipe[0], sync_cb);
        }
    }
    return 0;
}

void init_event_loop(void)
{
    epoll_fd = epoll_create1(0);
    if (epoll_fd  < 0) {
       printf("error init epoll\n");
       return;
    }
}

void start_event_loop(unsigned int number_of_dockers)
{
    int i;
    struct epoll_event events[MAX_EVENTS];
    int event_count;
    struct event_cb *cb;
    proccess_num = number_of_dockers;

    while(proccess_num) {
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 5000);
        if (event_count == -1) {
            perror("epoll_wait failed");
            return;
        }
        for (i = 0; i < event_count; ++i) {
            cb = (struct event_cb*)events[i].data.ptr;
            if (handle_epoll_read(cb->fd, cb->proccess_async) == -1) {
                printf("error in  cb\n");
            }
            free(cb);
        }
    }
}

void destroy_event_loop(void)
{
    close(epoll_fd);
}