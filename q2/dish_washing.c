/****
  * File containing code to solve dish washing problem
  *
  */

// Dependencies
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define MAX_TAPS 100
#define MAX_STUDENTS 100

sem_t tap_lock[MAX_TAPS];
int tap_count;
sem_t sb_lock[MAX_TAPS];
int sb_count;

float diff[MAX_STUDENTS];
time_t start_time;

void* begin(void* arg){
  int id = (uintptr_t)arg;
  int exit_flag = 1;
  while(1){
    for(int i=0;i<tap_count;++i){
      int res = sem_trywait(&tap_lock[i]);
      if(res != 0 && errno == EAGAIN){  // Failed to get tap
        exit_flag = 0;
      }else if(res == 0){ // Got access to tap
        int sb1 = i/2 >= 0?i/2:i/2+1;
        int sb2 = i/2+1 < sb_count?i/2+1:i/2;
        if(i==0){
          sb1 = 0;
          sb2 = 0;
        }
        if(i==tap_count-1){
          sb1 = sb_count-1;
          sb2 = sb_count-1;
        }
        printf("student %d got tap %d sb1 %d sb2 %d\n", id, i, sb1, sb2);
        while(1){
          int res1 = sem_trywait(&sb_lock[sb1]);
          if(res1 == 0){
            printf("student %d got sb %d\n", id, sb1);
            sleep(4);
            sleep(3);
            sleep(2);
            sem_post(&sb_lock[sb1]);
            sleep(5);
            sleep(3);
            sleep(1);
            sem_post(&tap_lock[i]);
            exit_flag = 1;
            break;
          }
          // if(res1 != 0 && errno == EAGAIN){
          //
          // }else if(res == 0){
          //   printf("student %d got sb %d\n", id, sb1);
          //   sleep(9);
          //   sem_post(&sb_lock[sb1]);
          //   sleep(9);
          //   sem_post(&tap_lock[i]);
          //   break;
          // }
          int res2 = sem_trywait(&sb_lock[sb2]);
          if(res2 == 0){
            printf("student %d got sb %d\n", id, sb2);
            sleep(4);
            sleep(3);
            sleep(2);
            sem_post(&sb_lock[sb2]);
            sleep(5);
            sleep(3);
            sleep(1);
            sem_post(&tap_lock[i]);
            exit_flag = 1;
            break;
          }
          // if(res1 != 0 && errno == EAGAIN){
          //
          // }else if(res == 0){
          //   printf("student %d got sb %d\n", id, sb2);
          //   sleep(9);
          //   sem_post(&sb_lock[sb2]);
          //   sleep(9);
          //   sem_post(&tap_lock[i]);
          //   break;
          // }
        }
        break;
      }
    }
    if(exit_flag){
      time_t end;
      time(&end);
      diff[id] = difftime(end, start_time);
      break;
    }
  }
}

int main(int argc, char* argv[]){
  if(argc != 3){
    printf("Usage: %s <no of taps> <no of students>\n", argv[0]);
    return 1;
  }

  tap_count = atoi(argv[1]);
  sb_count = tap_count-1;

  for(int i=0;i<tap_count;++i){
    sem_init(&tap_lock[i], 0, 1);
  }
  for(int i=0;i<sb_count;++i){
    sem_init(&sb_lock[i], 0, 1);
  }

  int student_count = atoi(argv[2]);
  pthread_t student_list[student_count];

  time(&start_time);

  // Start each student as a separate thread
  for(int i=0;i<student_count;++i){
    pthread_create(&student_list[i], NULL, begin, (void*)(uintptr_t)i);
  }
  // Wait for threads to finish execution
  for(int i=0;i<student_count;++i){
    pthread_join(student_list[i], NULL);
  }

  for(int i=0;i<tap_count;++i){
    sem_destroy(&tap_lock[i]);
  }
  for(int i=0;i<sb_count;++i){
    sem_destroy(&sb_lock[i]);
  }

  for(int i=0;i<student_count;++i){
    printf("student: %d time: %f\n", i, diff[i]);
  }
}
