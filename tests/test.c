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

int test_posix_spawn(void) {
    pid_t pid;
    const char *args[] = {"normal.sh", NULL};
    int err = ie_posix_spawn(&pid, "normal.sh", NULL, NULL, (char * const *)args, NULL);
    if (err != 0) {
        printf("\nposix_spawn failed: %s", strerror(errno));
    }
    waitpid(pid, NULL, 0);
    return err;
}

int test_system(void) {
    int err;
    err = ie_system(NULL);
    if (err != 0)
        puts("Interpreter is available");
    else {
        puts("Interpreter is NOT available");
        return 1;
    }

    err = ie_system("./normal.sh");
    if (err == -1)
        printf("\nsystem failed: %s", strerror(errno));
    else if (err == 127)
        printf("\nsystem failed to execute shell");
    return err;
}

int main(void) {
    puts("\tTESTING DIRECT FILE EXECUTION");

    int ret = test_direct_file();
    if (ret != 0) return ret;

    puts("\n\tTESTING posix_spawn");

    ret = test_posix_spawn();
    if (ret != 0) return ret;

    puts("\n\tTESTING system");

    ret = test_system();
    if (ret != 0) return ret;
}
