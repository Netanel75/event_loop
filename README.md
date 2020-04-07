# event_loop

## exec_lib: 
  This library demostrates how to create event loop in linux/c. 
  
  All processes are executed using execlp. 
  stderr/stdout redirected to "pipe write side" in the child process, and read from "pipe read side" in the parent.

  ### sync
  - Parent process reads from the pipe and wait for the child process to finish.
  - Call the user callback.
  ### async
   - Add the file descriptor and the cb to epoll.
   - Parent prcoess waits on epoll.
   - Call the user callback.

### ps_docker
  Simple main which uses exec library
  
### *compilation instrucations:*
    This project is compiled using: https://mesonbuild.com/
    meson builddir
    cd builddir 
    ninja
 
