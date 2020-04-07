# event_loop

## exec_lib: 
  This library demostrates how to create event loop in linux/c. 
  
  All processes are executed using execlp. 
  stderr/stdout redirected to "pipe write side" in the child process, and read from "pipe read side".
  .
  ### sync
  Read from the pipe and wait for the child process to finish
  ### async
  Add the file descriptor and the cb to epoll
  Read from epoll, and execute the cb

### ps_docker
  Simple main which uses exec library
  
### *compilation instrucations:*
    This project is compiled using: https://mesonbuild.com/
    meson builddir
    cd builddir 
    ninja
 
