#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void child_process(int fd_in, int fd_out)
{
  char buf;
  if (read(fd_in, &buf, 1) != 1) {
    fprintf(2, "pingpong: child read failed\n");
    exit(1);
  }
  printf("%d: received ping\n", getpid());
  if (write(fd_out, "x", 1) != 1) {
    fprintf(2, "pingpong: parent write failed\n");
    exit(1);
  }
  close(fd_in);
  close(fd_out);
}

void parent_process(int fd_in, int fd_out)
{
  char buf;
  if (write(fd_out, "x", 1) != 1) {
    fprintf(2, "pingpong: parent write failed\n");
    exit(1);
  }
  if (read(fd_in, &buf, 1) != 1) {
    fprintf(2, "pingpong: child read failed\n");
    exit(1);
  }
  printf("%d: received pong\n", getpid());

  close(fd_in);
  close(fd_out);
}

int
main(int argc, char *argv[])
{
  if(argc != 1){
    fprintf(2, "pingpong: invalid arguments\n");
    exit(1);
  }

  int pipefd_in[2];
  int pipefd_out[2];
  if (pipe(pipefd_in) < 0 || pipe(pipefd_out) < 0){
    fprintf(2, "pipe: pipe failed\n");
    exit(1);
  }

  int pid = fork();
  switch(pid) {
    case -1:
      exit(1);

    case 0:
      close(pipefd_in[0]);
      close(pipefd_out[1]);
      child_process(pipefd_out[0], pipefd_in[1]);
      exit(0);

    default:
      close(pipefd_in[1]);
      close(pipefd_out[0]);
      parent_process(pipefd_in[0], pipefd_out[1]);
      wait(0);
      break;
  }

  exit(0);
}
