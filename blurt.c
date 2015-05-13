 /*
 * Blurt v0.0
 *
 * A trivial multitasking filesystem load generator
 *
 * Daniel Phillips, June 2015
 *
 * to build: c99 -Wall blurt.c -oblurt
 * to run: blurt <basename> <steps> <tasks>
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

enum { chunk = 1024, sync = 0 };

char text[chunk] = { "hello world!\n" };

int main(int argc, const char *argv[]) {
        const char *basename = argc < 1 ? "foo" : argv[1];
        char name[100];
        int steps = argc < 3 ? 1 : atoi(argv[2]);
        int tasks = argc < 4 ? 1 : atoi(argv[3]);
        int fd, status, errors = 0;

        for (int t = 0; t < tasks; t++) {
                snprintf(name, sizeof name, "%s%i", basename, t);
                if (!fork())
                        goto child;
        }
        for (int t = 0; t < tasks; t++) {
                wait(&status);
                if (WIFEXITED(status) && WEXITSTATUS(status))
                        errors++;
        }
        return !!errors;

child:
        fd = creat(name, S_IRWXU);
        if (fd == -1)
                goto fail1;
        for (int i = 0; i < steps; i++) {
                int ret = write(fd, text, sizeof text);
                if (ret == -1)
                        goto fail2;
                if (sync)
                        fsync(fd);
        }
        return 0;
fail1:
        perror("create failed");
        return 1;
fail2:
        perror("write failed");


        return 1;
}
