// proj2.c
// Solution IOS-Project-2, 21.4.2024 
// Author: Michal Repčík, FIT
// Compiled: gcc 11.4.0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

// Arguments
typedef struct args {
    int L;     // Number of skiers
    int Z;     // Number of bus stops
    int K;     // Bus capacity
    int TL;    // Max waiting time at stop (ms)
    int TB;    // Max travel time between stops (ms)
} args_t;

// Shared variables
typedef struct shared_vars {
    int line_num;       // Line number
    int boarded;        // Number of skiers in the bus
    int *stops_arr;     // Associative array of stops ([bus_stop] = num_skiers_waiting)
    int current_stop;   // Stop that bus is currently waiting on
} shared_vars_t;

// Global variables
// Semaphores 
sem_t *mutex;       // Allow only one process to run
sem_t *file_sem;    // Handle file writes
sem_t *bus_depart;  // Handle bus departures
sem_t *enter_bus;   // Hande skiers boarding
sem_t *exit_bus;    // Handle skiers leaving
// File pointer
FILE *file;

// Function that parses cmd line argument and assigns values to varaibles
void parse_args(int argc, char* argv[], args_t *args) {
    if (argc != 6) {
        fprintf(stderr, "Incorrect number of command-line arguments.\n");
        exit(EXIT_FAILURE);
    }
    
    // Convert values to UL
    args->L = atoi(argv[1]);
    args->Z = atoi(argv[2]);
    args->K = atoi(argv[3]);
    args->TL = atoi(argv[4]);
    args->TB = atoi(argv[5]);

    // Check arguments
    if (args->L < 0 || args->L >= 20000) {
        fprintf(stderr, "Value L out of range.\n");
        exit(EXIT_FAILURE);
    }
    if (args->Z <= 0 || args->Z > 10) {
        fprintf(stderr, "Value Z out of range.\n");
        exit(EXIT_FAILURE);
    }
    if (args->K < 10 || args->K > 100) {
        fprintf(stderr, "Value K out of range.\n");
        exit(EXIT_FAILURE);
    }
    if (args->TL < 0 || args->TL > 10000) {
        fprintf(stderr, "Value TL out of range.\n");
        exit(EXIT_FAILURE);
    }
    if (args->TB < 0 || args->TB > 1000) {
        fprintf(stderr, "Value TB out of range.\n");
        exit(EXIT_FAILURE);
    }
}

void open_file() {
    if ((file = fopen("proj2.out","w")) == NULL){
        fprintf(stderr, "Failed to open a file\n");
        exit(EXIT_FAILURE);
    }
}

// Function that allocates and initializes all semaphores
void init_sems() {
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(mutex, 1, 1);
    file_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(file_sem, 1, 1);
    bus_depart = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(bus_depart, 1, 0);
    enter_bus = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(enter_bus, 1, 0);
    exit_bus = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    sem_init(exit_bus, 1, 0);
}

// Function that allocats and initializes all shared variables
shared_vars_t* init_shared_vars(args_t *args) {
    shared_vars_t *shared_vars = mmap(NULL, sizeof(shared_vars_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (shared_vars == MAP_FAILED) {
        perror("Failed to create shared memory");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for stops_arr and initialize elements to 0
    shared_vars->stops_arr = mmap(NULL, (args->Z + 1) * sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (shared_vars->stops_arr == MAP_FAILED) {
        perror("Failed to create shared memory for stops_arr");
        munmap(shared_vars, sizeof(shared_vars_t)); // Clean up memory allocated for shared_vars
        exit(EXIT_FAILURE);
    }

    // Initialize shared variables
    for (int i = 0; i <= args->Z; i++) {
        shared_vars->stops_arr[i] = 0;
    }
    shared_vars->line_num = 0;
    shared_vars->boarded = 0;
    shared_vars->current_stop = 0;

    return shared_vars;
}

// Function that destroys all semaphores and shared variables
void destroy_all(shared_vars_t *shared_vars) {
    // Destroy shared variables
    if (shared_vars != NULL) {
        munmap(shared_vars, sizeof(shared_vars_t));
    }
    // Destroy semaphores
    sem_destroy(mutex);
    munmap(mutex, sizeof(sem_t));
    sem_destroy(file_sem);
    munmap(file_sem, sizeof(sem_t));
    sem_destroy(bus_depart);
    munmap(bus_depart, sizeof(sem_t));
    sem_destroy(enter_bus);
    munmap(enter_bus, sizeof(sem_t));
    sem_destroy(exit_bus);
    munmap(exit_bus, sizeof(sem_t));
}

// Function that prints to the file
void file_print(const char *format, shared_vars_t *shared_vars, ...) {
    sem_wait(file_sem);
    va_list args;
    va_start(args, shared_vars);
    fprintf(file, "%d: ", ++shared_vars->line_num); // Increment before printing
    vfprintf(file, format, args);
    fflush(file);
    va_end(args);
    sem_post(file_sem);
}

// Function that handles bus process
void bus_process(args_t *args, shared_vars_t *shared_vars) {
    srand(time(NULL));
    file_print("BUS: started\n", shared_vars);

    int loop = 1;
    while (loop == 1) {
        for (int i = 1; i <= args->Z; i++) {
            usleep(rand() % args->TB + 1); // Travel to next stop

            sem_wait(mutex);
            shared_vars->current_stop = i;  // Change current stop
            file_print("BUS: arrived to %d\n", shared_vars, i);
            sem_post(mutex);

            // Let skiers board if there are any at the current stop
            if (shared_vars->stops_arr[i] > 0 && shared_vars->boarded < args->K) {
                sem_post(enter_bus);    // Signal skiers to enter bus
                sem_wait(bus_depart);      // Wait for signal to depart
            }
            file_print("BUS: leaving %d\n", shared_vars, i);
        }
        // Travel to final stop
        usleep(rand() % args->TB + 1);
        file_print("BUS: arrived to final\n", shared_vars);
        sem_post(exit_bus);     //Signal skiers to exit bus
        sem_wait(bus_depart);      // Wait for signal to depart
        file_print("BUS: leaving final\n", shared_vars);

        // Repeat loop if there are any skiers left on the stops
        loop = 0;
        for (int j = 1; j <= args->Z; j++) {
            if (shared_vars->stops_arr[j] > 0) {
                loop = 1;
            }
        }
    }

    file_print("BUS: finish\n", shared_vars);
    exit(EXIT_SUCCESS);
}

// Function that handles skiers
void skier_process(unsigned id, args_t *args, shared_vars_t *shared_vars) {
    srand(time(NULL) + id);
    file_print("L %d: started\n", shared_vars, id);

    // Sleep for random time
    usleep(rand() % (args->TB + 1));

    // Choose random stop
    int bus_stop = rand() % args->Z + 1;
    sem_wait(mutex);
    shared_vars->stops_arr[bus_stop]++; // Increment skiers at the stop
    file_print("L %d: arrived to %d\n", shared_vars, id, bus_stop);
    sem_post(mutex);
    
    
    int loop = 1;
    while (loop == 1) {
        sem_wait(enter_bus);
        // When current stop is the same as bus stop and bus is not full 
        if (shared_vars->current_stop == bus_stop && shared_vars->boarded < args->K) {
            file_print("L %d: boarding\n", shared_vars, id);
            shared_vars->boarded++;
            shared_vars->stops_arr[bus_stop]--;
            loop = 0; // Terminate loop

            // When current bus stop is empty or bus capacity is full signal bus
            if (shared_vars->stops_arr[bus_stop] == 0 || shared_vars->boarded == args->K) {
                sem_post(bus_depart);      // Signal bus to depart
            }
            else {
                sem_post(enter_bus);    // Signal others to enter
            }
        }
        else {
            sem_post(enter_bus);        // Signal others to enter
        }
    }

    // Wait for signal to exit the bus
    sem_wait(exit_bus);
    shared_vars->boarded--;
    file_print("L %d: going to ski\n", shared_vars, id);
    if (shared_vars->boarded == 0) {
        sem_post(bus_depart);  // Signal bus to depart
    }
    else {
        sem_post(exit_bus); // Signal others to exit
    }

    exit(EXIT_SUCCESS);
}

// Function that handles main process
void main_process(args_t *args, shared_vars_t *shared_vars) {
    pid_t pid;

    // Create bus process
    pid = fork();
    if (pid == 0) {
        bus_process(args, shared_vars);
        exit(0);
    } 
    else if (pid < 0) {
        perror("Failed to create bus process");
        destroy_all(shared_vars);
        exit(EXIT_FAILURE);
    }

    // Create skier processes
    for (unsigned i = 1; i <= (unsigned)args->L; i++) {
        pid = fork();
        if (pid == 0) {
            skier_process(i, args, shared_vars);
            exit(0);
        } 
        else if (pid < 0) {
            perror("Failed to create skier process");
            destroy_all(shared_vars);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all child processes to finish
    while (wait(NULL) > 0);
}

// Main function
int main(int argc, char* argv[]) {
    // Initialize structs
    args_t args;
    shared_vars_t *shared_vars;
    // Parse arguments
    parse_args(argc, argv, &args);
    // Open file
    open_file();
    // Create semaphores and shared variables
    shared_vars = init_shared_vars(&args);
    init_sems();
    // Run main process
    main_process(&args, shared_vars);
    // Clean up semaphores and shared variables
    destroy_all(shared_vars);
    return 0;
}
