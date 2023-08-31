/* File:  
 *    etapa2.c
 *
 * Purpose:
 *    Implementação do modelo produtor consumidor
 *
 *
 * Compile:  gcc -g -Wall -o etapa2 etapa2.c -lpthread -lrt
 * Usage:    ./etapa2
 *
 *Charlie Rodrigues Fonseca
 *Elana Tanan Sande
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//#define THREAD_NUM 4    // Tamanho do pool de threads
#define THREAD_NUM 6
#define BUFFER_SIZE 256 // Númermo máximo de tarefas enfileiradas

/*
typedef struct Task{
   int a, b;
}Task;
*/
typedef struct Clock { 
   int p[3];
} Clock;


/*
Task taskQueue[BUFFER_SIZE];
int taskCount = 0;
*/
Clock clockQueue[BUFFER_SIZE];
int clockCount = 0;

pthread_mutex_t mutex;

pthread_cond_t condFull;
pthread_cond_t condEmpty;
/*
void executeTask(Task* task, int id){
   int result = task->a + task->b;
   printf("(Thread %d) Sum of %d and %d is %d\n", id, task->a, task->b, result);
}
*/
void printClock(Clock* clock, int id){
   printf("a)Process: %d, Clock: (%d, %d, %d)\n", id, clock->p[0], clock->p[1], clock->p[2]);
}
/*
Task getTask(){
   pthread_mutex_lock(&mutex);
   
   while (taskCount == 0){
      pthread_cond_wait(&condEmpty, &mutex);
   }
   
   Task task = taskQueue[0];
   int i;
   for (i = 0; i < taskCount - 1; i++){
      taskQueue[i] = taskQueue[i+1];
   }
   taskCount--;
   
   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&condFull);
   return task;
}
*/
Clock consumeClock(){
   pthread_mutex_lock(&mutex);
   
   while (clockCount == 0){
      pthread_cond_wait(&condEmpty, &mutex);
   }
   
   Clock clock = clockQueue[0];
   int i;
   for (i = 0; i < clockCount - 1; i++){
      clockQueue[i] = clockQueue[i+1];
   }
   clockCount--;
   
   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&condFull);
   return clock;
}
/*
void submitTask(Task task){
   pthread_mutex_lock(&mutex);

   while (taskCount == BUFFER_SIZE){
      pthread_cond_wait(&condFull, &mutex);
   }

   taskQueue[taskCount] = task;
   taskCount++;

   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&condEmpty);
}
*/
void produceClock(Clock clock){
   pthread_mutex_lock(&mutex);

   while (clockCount == BUFFER_SIZE){
      pthread_cond_wait(&condFull, &mutex);
   }

   clockQueue[clockCount] = clock;
   clockCount++;

   pthread_mutex_unlock(&mutex);
   pthread_cond_signal(&condEmpty);
}

Clock updateClock(Clock* clock, long id){
   clock->p[id]++;
   return *clock;
}

void *startThread(void* args);  

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   pthread_mutex_init(&mutex, NULL);
   
   pthread_cond_init(&condEmpty, NULL);
   pthread_cond_init(&condFull, NULL);

   pthread_t thread[THREAD_NUM]; 
   long i;
   for (i = 0; i < THREAD_NUM; i++){  
      if (pthread_create(&thread[i], NULL, &startThread, (void*) i) != 0) {
         perror("Failed to create the thread");
      }  
   }
   /*
   srand(time(NULL));
   for (i = 0; i < 500; i++){
      Task t = {
         .a = rand() % 100,
         .b = rand() % 100
      };
      submitTask(t);
   }
   
   for (i = 0; i < THREAD_NUM; i++){  
      if (pthread_join(thread[i], NULL) != 0) {
         perror("Failed to join the thread");
      }  
   }
   */
   
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&condEmpty);
   pthread_cond_destroy(&condFull);
   return 0;
}  /* main */

/*-------------------------------------------------------------------*/
/*
void *startThread(void* args) {
   long id = (long) args;
   
  while (1){ 
      Task task = getTask();
      executeTask(&task, id);
      sleep(rand()%5);
   }
  
   return NULL;
} 
*/
void *startThread(void* args) {
   long id = (long) args;
   Clock clock = {{0,0,0}};
   
   //Produtores
   if (id == 0 || id == 1 || id == 2){
      while (1){ 
         clock = updateClock(&clock, id);
         produceClock(clock);
         sleep(rand()%5);
      }
   } else {
      while (1){ 
         clock = consumeClock();
         printClock(&clock, id);
         sleep(rand()%5);
      }
   }
   
   
   return NULL;
}
