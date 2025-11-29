# Raptee Assignment – Embedded Software Intern  
This repository contains my submission for the **Embedded Software Engineer – Intern** assignment for Raptee.HV.

## Overview  
The task simulates a two-thread software similar to what is used in EV and real-time telemetry systems:

- **Fast Loop (100 Hz)**  
  - Reads simulated sensor data (battery %, wheel speed, temperature)  
  - Computes distance travelled incrementally  
  - Tracks top speed  
  - Pushes each reading into a thread-safe FIFO queue

- **Slow Loop**  
  - Dequeues sensor data from the buffer  
  - Sends updates to a simulated cloud endpoint under the following conditions:  
    - **Battery Update:** When the vehicle is stationary and battery percentage changes  
    - **Critical Alert:** When temperature crosses a defined threshold  
    - **Regular Update:** Once every second when vehicle is moving

## Technical Highlights  
- Implemented using **POSIX threads (pthread)**  
- Utilizes a **mutex-protected circular queue** for producer–consumer communication  
- **Random blocking delay (1–10 seconds)** added to simulate real network/cloud latency  
- Accumulates **total distance** and tracks **maximum speed** over time  
- Queue uses an **overwrite-oldest** behavior when full to avoid blocking the fast loop

## Compilation  
Run the following command:

```bash
gcc -std=gnu11 -O2 -Wall -pthread raptee_task.c -o raptee_task
```

## Execution  
```bash
./raptee_task
```

## Notes  
- This implementation is designed as a simulation for the assignment and not intended for production deployment.  
- Console logs include sensor readings, computed values, and cloud upload events for clarity.
