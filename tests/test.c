#include <libiosexec.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

int main(void) {
  char *binaryPath = "./example.sh";
  char *arg1 = "arg1";
  char *arg2 = "arg2";
  char *args[] = {binaryPath, arg1, arg2, NULL};

  ie_execv(binaryPath, args);
  printf("execv test failed.");
  return 0;
}
