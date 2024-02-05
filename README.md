# OS-assignments
This repository includes five carefully crafted assignments for the "Operating Systems" course. The programs are written in C or C++ and are specifically designed to run smoothly on Linux systems. Avoid using alternative platforms like Windows.
<br>
<img src= "https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/C_Programming_Language.svg/926px-C_Programming_Language.svg.png" width = "100" height="100">
<img src= "https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/ISO_C%2B%2B_Logo.svg/1200px-ISO_C%2B%2B_Logo.svg.png" width = "100" height="100">

### 1. First assignment
<p align="justify">
First assignment is about signals in operating sistem. The first assignment contains one program, and it works on POSIX and Linux operating sistems. 
  
* This C++ program simulates a simple operating system scenario with interrupt handling. It defines a set of constants and arrays to manage interrupt priorities and waiting queues. The main program loop represents the basic system operations. When a user-triggered interrupt (SIGINT) occurs, it invokes the prekidnaRutina (interrupt service routine) to handle the interrupt by setting the appropriate priority and processing the interrupt in a separate function, obrada_prekida. The program then continues its main loop, showcasing a basic system operation for a specified duration. The priorities are managed, and interrupts are processed accordingly, mimicking a simplified operating system environment.
</p>

### 2. Second assignment
<p align="justify">
Second assignment is about proceses and threads. It contains two programs, one is using threads to simulate Dekkers algorithm, the other is using proceses. 
  
* This C++ program with proceses demonstrates interprocess communication using shared memory for parallel processing. It starts by creating a shared memory segment (kljucic) and attaches it to a 2D array (polje) to store randomly generated values. The program then forks M processes to concurrently generate and calculate sums and averages for different sections of the array. Each child process either performs data generation (generiranje) or data calculation (racunanje). The main program waits for all child processes to finish using the wait system call and then detaches and removes the shared memory segment. The program is designed to handle interrupts (SIGINT), ensuring proper cleanup of shared memory resources in case of an interrupt signal. In summary, this program showcases a basic parallel computing scenario using shared memory for communication between processes, where each process contributes to the generation and calculation of data in a shared array.

* The second C++ program demonstrates the implementation of the Dekker's algorithm, a mutual exclusion algorithm for two processes. It uses threads to simulate two concurrent processes attempting to enter a critical section. The program initializes flags and counters to manage the critical section entry, and the udji_u_kriticni_odsjecak and izadji_iz_kriticnog_odsjecka functions handle the entry and exit of the critical section. The proc function represents the critical section code, and the main function creates and joins threads to simulate concurrent execution. In summary, the program showcases mutual exclusion using Dekker's algorithm in a multithreaded environment.
</p>

### 3. Third assignment
<p align="justify">
Third assignment is about semaphores. Contains two programs, one is about missionares and cannibals problem, the other is about supply and demand.

* First one uses unnamed semaphores second one uses named semaphores. This C++ program with cannibals simulates the "Missionaries and Cannibals" problem using pthreads and semaphores for synchronization. The main elements include two types of passengers (Missionaries and Cannibals), a boat (cun), and two shores. The program ensures that the number of passengers in the boat adheres to certain rules: the number of Cannibals should not exceed the number of Missionaries. The simulation continues indefinitely, creating threads for passengers and the boat, and handling synchronization using semaphores. The program displays the movements of passengers and the state of the boat in the console output.

* The second C++ program with named semaphores uses shared memory for parallel processing. It creates a shared memory segment and attaches it to a 2D array to store randomly generated values. The program then forks M processes to concurrently generate and calculate sums and averages for different sections of the array. Each child process either performs data generation (generiranje) or data calculation (racunanje). The main program waits for all child processes to finish using the wait system call and then detaches and removes the shared memory segment. The program is designed to handle interrupts (SIGINT), ensuring proper cleanup of shared memory resources in case of an interrupt signal. In summary, this program demonstrates parallel computing with shared memory communication for data generation and calculation.
</p>

### 4. Forth assignment
<p align="justify">
Forth assignment is displaying barrier problem and sleeping barber problem.

* The first one implements a simple barrier mechanism using pthreads. It allows a specified number of threads to wait until all threads have reached the barrier before proceeding. Each thread inputs a number, and the program prints the entered numbers after all threads have reached the barrier. The program checks for the correct number of command-line arguments, initializes pthread structures, creates threads, and ensures that each thread waits for others at the barrier. It also handles the SIGINT signal for clean termination.

* This C program simulates a barbershop scenario with multiple clients and a barber using pthreads. The program initializes pthread structures and demonstrates synchronization using mutex and condition variables. The main components include a generator for creating clients, a barber, and a loop that generates clients during the working hours. Clients enter a waiting area, and when a seat is available, they get serviced by the barber.
</p>

### 5. Fifth assignment
<p align="justify">
Fifth assignment is about pagination. There are two ways that are implemented FIFO and LFU.
  
* The first program implements an LFU (Least Frequently Used) cache algorithm in C++. It simulates a cache system with a specified size, tracking the frequency and order of page accesses.

* The second program implements a First In, First Out (FIFO) cache, a simple caching algorithm that removes the oldest accessed item when the cache reaches its limit. FIFO follows the principle of "first come, first served."
</p>

