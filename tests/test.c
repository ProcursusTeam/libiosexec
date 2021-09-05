#include <errno.h>
#include <libiosexec.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

int test_direct_file(void) {
    puts("Testing normal script...");
    int pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            return 1;
            break;
        case 0:
            ie_execl("normal.sh", "normal.sh", NULL);
            printf("FAILURE to exec at line %d: %s", __LINE__, strerror(errno));
            break;
        default:
            waitpid(pid, NULL, 0);
    }

    puts("Testing empty script...");
    pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            return 1;
            break;
        case 0:
            ie_execl("empty.sh", "empty.sh", NULL);
            printf("FAILURE to exec at line %d: %s", __LINE__, strerror(errno));
            break;
        default:
            waitpid(pid, NULL, 0);
    }

    puts("Testing script with singular arg...");
    pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            return 1;
            break;
        case 0:
            ie_execl("normalwitharg.sh", "normalwitharg.sh", NULL);
            printf("FAILURE to exec at line %d: %s", __LINE__, strerror(errno));
            break;
        default:
            waitpid(pid, NULL, 0);
    }

    puts("Testing script with multiple args...");
    pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            return 1;
            break;
        case 0:
            ie_execl("normalwithmultipleargs.sh", "normalwithmultipleargs.sh", NULL);
            printf("FAILURE to exec at line %d: %s", __LINE__, strerror(errno));
            break;
        default:
            waitpid(pid, NULL, 0);
    }

    return 0;
}

int main(void) {
    puts("\tTESTING DIRECT FILE EXECUTION");

    int ret = test_direct_file();
    if (ret) return ret;

    puts("\n\tTESTING PATH RESOLVER");
}
