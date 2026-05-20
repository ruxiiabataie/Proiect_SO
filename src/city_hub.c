#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    char command[256];

    while (1)
    {
        printf("city_hub> ");
        fflush(stdout);

        if (fgets(command,
                  sizeof(command),
                  stdin) == NULL)
        {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        /* EXIT */

        if (strcmp(command,
                   "exit") == 0)
        {
            printf("Exiting city_hub...\n");

            break;
        }

        /* START MONITOR */

        else if (strcmp(command,
                         "start_monitor") == 0)
        {
            int fd[2];

            if (pipe(fd) == -1)
            {
                perror("pipe");

                continue;
            }

            pid_t pid = fork();

            if (pid < 0)
            {
                perror("fork");

                continue;
            }

            if (pid == 0)
            {
                close(fd[0]);

                dup2(fd[1],
                     STDOUT_FILENO);

                close(fd[1]);

                execl("./monitor_reports",
                      "monitor_reports",
                      NULL);

                perror("execl");

                exit(1);
            }

            close(fd[1]);

            sleep(1);

            char buffer[1024];

            int bytesRead = read(fd[0],
                                 buffer,
                                 sizeof(buffer) - 1);

            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';

                printf("FROM MONITOR: %s\n",
                       buffer);

                if (strstr(buffer,
                           "ERROR") == NULL)
                {
                    printf("Monitor launched successfully\n");
                }
            }

            close(fd[0]);
        }

        /* STOP MONITOR */

        else if (strcmp(command,
                         "stop_monitor") == 0)
        {
            FILE *fp;

            int pid;

            fp = fopen(".monitor_pid",
                       "r");

            if (fp == NULL)
            {
                printf("No monitor is running\n");

                continue;
            }

            fscanf(fp,
                   "%d",
                   &pid);

            fclose(fp);

            if (kill(pid,
                     SIGINT) == 0)
            {
                printf("Monitor stopped successfully\n");

                unlink(".monitor_pid");

                sleep(1);
            }
            else
            {
                perror("kill");
            }
        }

        /* CALCULATE SCORES */

        else if (strncmp(command,
                         "calculate_scores",
                         16) == 0)
        {
            char *token;

            token = strtok(command, " ");

            token = strtok(NULL, " ");

            while (token != NULL)
            {
                printf("Processing district: %s\n",
                       token);

                int fd[2];

                if (pipe(fd) == -1)
                {
                    perror("pipe");

                    break;
                }

                pid_t pid = fork();

                if (pid < 0)
                {
                    perror("fork");

                    break;
                }

                if (pid == 0)
                {
                    close(fd[0]);

                    dup2(fd[1],
                         STDOUT_FILENO);

                    close(fd[1]);

                    execl("./scorer",
                          "scorer",
                          token,
                          NULL);

                    perror("execl");

                    exit(1);
                }

                close(fd[1]);

                char buffer[1024];

                int bytesRead = read(fd[0],
                                     buffer,
                                     sizeof(buffer) - 1);

                if (bytesRead > 0)
                {
                    buffer[bytesRead] = '\0';

                    printf("%s\n",
                           buffer);
                }

                close(fd[0]);

                token = strtok(NULL, " ");
            }
        }

        /* UNKNOWN COMMAND */

        else
        {
            printf("Unknown command\n");
        }
    }

    return 0;
}