#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define PID_FILE ".monitor_pid"

void handle_sigusr1(int sig)
{
    (void)sig;

    write(STDOUT_FILENO,
          "New report added!\n",
          18);
}

void handle_sigint(int sig)
{
    (void)sig;

    write(STDOUT_FILENO,
          "Monitor shutting down...\n",
          26);

    unlink(PID_FILE);

    _exit(0);
}

int main()
{
    int fd;
    char buffer[32];
    int old_pid;

    struct sigaction sa_usr1, sa_int;

    /* verifică dacă există deja monitor */

    FILE *fp = fopen(PID_FILE, "r");

    if (fp != NULL)
{
    fscanf(fp, "%d", &old_pid);

    fclose(fp);

    if (kill(old_pid, 0) == 0)
    {
        printf("ERROR: Monitor already running with PID %d\n",
               old_pid);

        fflush(stdout);

        return 1;
    }
    else
    {
        unlink(PID_FILE);
    }
}

    /* scrie PID-ul nou */

    fd = open(PID_FILE,
              O_WRONLY | O_CREAT | O_TRUNC,
              0644);

    if (fd < 0)
    {
        perror("open");

        return 1;
    }

    snprintf(buffer,
             sizeof(buffer),
             "%d\n",
             getpid());

    write(fd,
          buffer,
          strlen(buffer));

    close(fd);

    /* handler SIGUSR1 */

    memset(&sa_usr1,
           0,
           sizeof(sa_usr1));

    sa_usr1.sa_handler = handle_sigusr1;

    sigemptyset(&sa_usr1.sa_mask);

    sa_usr1.sa_flags = 0;

    if (sigaction(SIGUSR1,
                  &sa_usr1,
                  NULL) < 0)
    {
        perror("sigaction SIGUSR1");

        return 1;
    }

    /* handler SIGINT */

    memset(&sa_int,
           0,
           sizeof(sa_int));

    sa_int.sa_handler = handle_sigint;

    sigemptyset(&sa_int.sa_mask);

    sa_int.sa_flags = 0;

    if (sigaction(SIGINT,
                  &sa_int,
                  NULL) < 0)
    {
        perror("sigaction SIGINT");

        return 1;
    }

    printf("Monitor started. PID: %d\n",
           getpid());

    fflush(stdout);

    /* monitorul rămâne activ */

    while (1)
    {
        pause();
    }

    return 0;
}