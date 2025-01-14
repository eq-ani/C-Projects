#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>

// global variable, tracks core status
volatile sig_atomic_t msg_num[3] = {0, 0, 0}; // holds signals from cores (0 idle, 1 busy)

// helper function for uninterrupted sleep
void no_interrupt_sleep(int sec)
{
    struct timespec req, rem;
    req.tv_sec = sec;
    req.tv_nsec = 0;

    while (nanosleep(&req, &rem) == -1 && errno == EINTR)
    {
        req = rem;
    }
}

// signal handler for task completion
void sigHandle(int signal, siginfo_t *sig, void *con)
{
    int index = signal - SIGRTMIN; // the core "index", (0 indexed)
    if (index >= 0 && index < 3)
    {
        msg_num[index] = 1; // mark "index" as idle
    }
}

// function to set up signal handling
void signalSetup()
{
    struct sigaction sigact;
    sigact.sa_sigaction = sigHandle;
    sigact.sa_flags = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);

    for (int i = 0; i < 3; i++) // error check for signal setup
    {
        if (sigaction(SIGRTMIN + i, &sigact, NULL) == -1)
        {
            perror("sigaction");
            exit(1);
        }
    }
    printf("Signals for task completion set up successfully.\n");
}

// function to create pipes
void create_pipes(int main_to_core[3][2], int core_to_main[3][2])
{
    for (int i = 0; i < 3; i++)
    {
        if (pipe(main_to_core[i]) == -1 || pipe(core_to_main[i]) == -1)
        {
            fprintf(stderr, "Pipe creation failed for core. %d\n", i + 1);
            exit(1);
        }
        printf("Pipes for core %d created successfully.\n", i + 1);
    }
}

// function to create subprocesses (cores)
void create_cores(int main_to_core[3][2], int core_to_main[3][2], int maxSleepTime)
{
    for (int i = 0; i < 3; i++)
    {
        pid_t pid = fork();
        if (pid == 0) // child process (core)
        {
            close(main_to_core[i][1]); // close unused write end
            close(core_to_main[i][0]); // close unused read end

            // close other cores' pipes
            for (int j = 0; j < 3; j++)
            {
                if (j != i)
                {
                    close(main_to_core[j][0]);
                    close(main_to_core[j][1]);
                    close(core_to_main[j][0]);
                    close(core_to_main[j][1]);
                }
            }

            srand(i ^ getpid() ^ rand() ^ 123456789 ^ time(NULL)); // random seed to simulate processing times
            int id = 0; // task ID
            int count = 0; // count of tasks processed by this core

            while (read(main_to_core[i][0], &id, sizeof(int)) > 0)
            {
                printf("Task %d being processed by core %d... \n", id + 1, i + 1);
                no_interrupt_sleep(rand() % maxSleepTime + 1); // randomized sleep time
                write(core_to_main[i][1], &id, sizeof(int)); // notify main process of completion
                kill(getppid(), SIGRTMIN + i); // send signal for task completion
                count++;
            }
            exit(count); // exit with count of tasks processed
        }
        else if (pid > 0)
        {
            printf("Core %d created successfully.\n", i + 1);
        }
    }
}

// function to close all pipes
void close_pipes(int main_to_core[3][2], int core_to_main[3][2])
{
    for (int i = 0; i < 3; i++)
    {
        close(main_to_core[i][1]);
        close(core_to_main[i][1]);
        close(main_to_core[i][0]);
        close(core_to_main[i][0]);
    }
    printf("All pipes closed.\n");
}

// function to reap child processes
void reap_children()
{
    for (int i = 0; i < 3; i++)
    {
        int coreStatus;
        wait(&coreStatus);
        if (WIFEXITED(coreStatus))
        {
            printf("Core %d processed %d tasks.\n", i + 1, WEXITSTATUS(coreStatus));
        }
    }
    printf("All child processes reaped.\n");
}

// *MAIN FUNCTION*: assigns tasks and coordinates the cores
int main(int argnum, char *args[])
{
    if (argnum != 3) // validate number of arguments
    {
        fprintf(stderr, "Wrong number of arguments, enter the number of tasks and max sleep time. ");
        exit(1);
    }

    int numTasks = atoi(args[1]); // total number of tasks
    int maxSleepTime = atoi(args[2]); // maximum sleep time per task

    if (numTasks <= 0 || maxSleepTime <= 0) // validate sign of arguments
    {
        fprintf(stderr, "Both number of tasks and max sleep time must be positive.\n");
        exit(1);
    }

    int main_to_core[3][2]; // pipes from main to cores
    int core_to_main[3][2]; // pipes from cores to main
    int completedTasks = 0; // number of tasks completed
    int assignedTasks = 0;  // number of tasks assigned

    signalSetup(); // initialize signal handling
    create_pipes(main_to_core, core_to_main); // create pipes
    create_cores(main_to_core, core_to_main, maxSleepTime); // create cores

    // main task assignment and completion monitoring loop
    while (completedTasks < numTasks)
    {
        for (int i = 0; i < 3; i++) // assign tasks to idle cores
        {
            if (msg_num[i] == 0 && assignedTasks < numTasks)
            {
                printf("Task %d assigned to core %d. \n", assignedTasks + 1, i + 1);
                if (write(main_to_core[i][1], &assignedTasks, sizeof(int)) != -1)
                {
                    assignedTasks++;
                    msg_num[i] = 1; // mark core as busy
                }
            }
        }

        for (int i = 0; i < 3; i++) // check for completed tasks
        {
            if (msg_num[i] > 0) 
            {
                int completed;
                if (read(core_to_main[i][0], &completed, sizeof(int)) > 0)
                {
                    printf("Task %d completed by core %d. \n", completed + 1, i + 1);
                    msg_num[i] = 0; // mark core as idle
                    completedTasks++;
                }
            }
        }
    }

    close_pipes(main_to_core, core_to_main); // close pipes
    reap_children(); // reap child processes

    return 0;
}
