Overview
This project simulates a simplified version of parallel processing using processes, pipes, and signals in C. 
It demonstrates how tasks can be assigned to subprocesses (cores), executed concurrently, and completed in a coordinated manner. 
The program takes two command-line arguments: the total number of tasks and the maximum sleep time for each task.

1. Program Requirements

Processes and Pipes:
Three subprocesses (cores) are created using fork() to simulate parallel processing.
Pipes are used for two-way communication between the main process and the cores.
Each core has its own pair of pipes:
main_to_core[i]: Main sends tasks to core.
core_to_main[i]: Core sends task completion status to main.

Signal Handling:
Real-time signals (SIGRTMIN to SIGRTMIN+2) are used for non-blocking task monitoring.
Signals are registered and handled correctly using sigaction().
Signal handlers ensure atomicity by updating a global volatile sig_atomic_t variable.

Task Assignment:
The main process assigns tasks to idle cores.
Each core processes tasks independently and notifies the main process upon task completion.

Post-Processing:
All pipes are closed properly.
Child processes (cores) are reaped after they finish their execution.


2. Design and Implementation

main():
Handles input validation.
Coordinates task assignment and completion monitoring.
Calls helper functions for setup, task management, and cleanup.

signalSetup():
Sets up real-time signal handling using sigaction().

create_pipes():
Creates two pipes for each core (one for sending tasks and one for receiving results).

create_cores():
Forks three subprocesses to simulate cores.
Each core:
Waits for a task from the main process.
Processes the task with a random delay (up to maxSleepTime).
Notifies the main process via pipes and signals.

close_pipes():
Closes all pipes after task execution.

reap_children():
Waits for child processes to terminate and logs the number of tasks processed by each core.

sigHandle():
Signal handler that updates the global msg_num array to track core statuses.

no_interrupt_sleep():
Ensures uninterrupted sleep for cores by handling system call interruptions (EINTR).

3. Output and Logs 
Assume a random run of "./main 10 5"
Output consists of:
-Core total tasks processed
"Core 1 processed 3 tasks.
Core 2 processed 3 tasks.
Core 3 processed 4 tasks."
Logs consist of:
-Signal set-up
"Signals for task completion set up successfully."
-Pipe Creation
"Pipes for core 1 created successfully.
Pipes for core 2 created successfully.
Pipes for core 3 created successfully."
-Core Creation
"Core 1 created successfully.
Core 2 created successfully.
Core 3 created successfully."
-Task Assignment 
"Task 1 assigned to core 1. 
Task 2 assigned to core 2. 
Task 3 assigned to core 3."
-Task Processing
"Task 2 being processed by core 2... 
Task 3 being processed by core 3... 
Task 1 being processed by core 1..." 
-Task Completion
"Task 3 completed by core 3."
-Pipe Closure
"All pipes closed."
-Child Process Reaping
"All child processes reaped."

