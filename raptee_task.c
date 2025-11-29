//raptee tasks - Sundar S 
//game plan is to create threads for fast loop(sensor readings) and slow loop(send to cloud). as data from sensor comes at a faster rate, buffer it in a queue and send to cloud. 
//we'll be using mutex to avoid race condition amongst the concurrent threads. Not using any RTOS specific features as we only have 2 tasks here. Instead, we'll use POSIX threads.

//overall algorithm -----> fast loop gives sensor data -> enqueue -> slow loop consumes -> dequeue -> send to cloud 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <math.h> 

//defining a sensor datatype
typedef struct {
    int battery_percentage;
    int wheel_speed;
    int temperature;
} SensorData;

SensorData sensor_data; 

//mutex
pthread_mutex_t mutex1;

//queue for tasks
#define MAX_TASKS 1000
SensorData queue[MAX_TASKS];
int front = 0, rear = 0;

//enqeue deqeue funtcion - only these require the mutex lock, otherwise the other 2 loops can run concurrently
void enqueue(SensorData data) {
    pthread_mutex_lock(&mutex1);

    if ((rear + 1) % MAX_TASKS != front) { //overflow check
        queue[rear] = data;
        rear = (rear + 1) % MAX_TASKS;
    }
    pthread_mutex_unlock(&mutex1);
}

void dequeue(SensorData* data) {
    pthread_mutex_lock(&mutex1);

    if (front != rear) { //underflow check
        *data = queue[front];
        front = (front + 1) % MAX_TASKS;
    }
    pthread_mutex_unlock(&mutex1);
}

int distance_travelled = 0;
int top_speed = 0;
//fast loop - sensor readings
void* fast_loop(void* arg) {
    while (1) {
        //simulating sensor readings
        sensor_data.battery_percentage = rand()%101;
        sensor_data.wheel_speed = rand()%201;
        sensor_data.temperature = rand()%151; 
        printf("Fast Loop - Sensor Readings: Battery: %d%%, Wheel Speed: %d km/h, Temperature: %d C\n", 
               sensor_data.battery_percentage, sensor_data.wheel_speed, sensor_data.temperature);

        //calculating other parameters from sensor data
        distance_travelled += sensor_data.wheel_speed * 0.01; //100Hz corresponds to 0.01 seconds
        if(sensor_data.wheel_speed > top_speed){
            top_speed = sensor_data.wheel_speed;
        }
        printf("Calculated - Distance Travelled: %d km, Top Speed: %d km/h\n", distance_travelled, top_speed);
        
        //adding to queue
        enqueue(sensor_data);

        usleep(10000); //(100Hz) 10ms delay
    }
    return NULL;
}

//slow loop - send to cloud
BATTERY_UPDATE = 1;
CRITICAL_ALERT = 2;
REGULAR_UPDATE = 3;
long now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}
int prev_battery = -1;
int last_sent = 0;

void* slow_loop(void* arg) {
    while(1){
        //check if queue is not empty
            SensorData send_data;
            dequeue(&send_data);

            volatile int stationary = send_data.wheel_speed == 0 ? 1 : 0;
            volatile int curr_battery = send_data.battery_percentage;
            volatile int temp = send_data.temperature;

            //vechile idle - send battery status only
            if(stationary && (prev_battery != curr_battery)){
                send_to_cloud(BATTERY_UPDATE, send_data);
                prev_battery = curr_battery;
            }

            else if(!stationary && temp>70){ //critical temp alert if temp greater than 70C
                send_to_cloud(CRITICAL_ALERT, send_data);
            }

            else if(!stationary && now()-last_sent >= 1){ //send every second if moving
                send_to_cloud(REGULAR_UPDATE, send_data);
                last_sent = now();
            }


        usleep(100000); //100 ms delay
    }
    return NULL;
}

int send_to_cloud(int type, SensorData data){
    //random delay
    int t = (rand() % 10) + 1;
    sleep(t);
    return 1; //always successful    
}

int main(){
    pthread_t fast_thread, slow_thread;

    pthread_mutex_init(&mutex1, NULL);

    pthread_create(&fast_thread, NULL, fast_loop, NULL);
    pthread_create(&slow_thread, NULL, slow_loop, NULL);

    pthread_join(fast_thread, NULL);
    pthread_join(slow_thread, NULL);

    pthread_mutex_destroy(&mutex1);

    return 0;
}