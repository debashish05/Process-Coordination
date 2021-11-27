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

#define MAX_TAPS 1000
#define MAX_STUDENTS 1000

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
        //printf("student %d got tap %d sb1 %d sb2 %d\n", id, i, sb1, sb2);
        while(1){
          int res1 = sem_trywait(&sb_lock[sb1]);
          if(res1 == 0){
            //printf("student %d got sb %d\n", id, sb1);
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
           // printf("student %d got sb %d\n", id, sb2);
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
  return 0;
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


// #include <iostream>
// #include <vector>
// #include <unistd.h>
// #include <queue>
// #include <pthread.h>
// #include <bits/stdc++.h>
// #include <pthread.h>
// #include <stdio.h>
// #include <time.h>
// #include <stdlib.h>
// #include <string.h>
// using namespace std;

// int waitingForTap = 0;
// int waitingForSB = 1;
// int usingSB = 2;
// int washing = 3;

// pthread_mutex_t lock1;

// vector<int> state;
// vector<int> studentId;
// vector<int> tap;
// queue<int> waitingQueue;
// vector<int> completionTime;
// vector<int> SB;
// unordered_map<int, int> tapAssig;
// unordered_map <int, int> SBmap;

// clock_t currTime;

// void *task(void *sId)
// {
//     clock_t ThreadcurrTime =currTime;
//     int studentId = *((int *)sId);
//     int tapNo = tapAssig[studentId];
//     pthread_mutex_lock(&lock1);
//     cout << "TAP NO: " << tapNo << " "<< "Student id: " << studentId << endl;
//     pthread_mutex_unlock(&lock1);
   
    
//     int SBindex=-1;

//     while (SBindex==-1)
//     {
//         if (tapNo == 0)
//         {
//             pthread_mutex_lock(&lock1);
//             if (SB[tapNo] == -1)
//             {
//                 SB[tapNo] = studentId;
//                 SBindex = tapNo;
//             }
//             pthread_mutex_unlock(&lock1);
//         }
//         else if (tapNo == tap.size() - 1)
//         {
//             pthread_mutex_lock(&lock1);
//             if (SB[tapNo - 1] == -1)
//             {
//                 SB[tapNo - 1] = studentId;
//                 SBindex = tapNo-1;
//             }
//             pthread_mutex_unlock(&lock1);
//         }
//         else
//         {
//             pthread_mutex_lock(&lock1);
//             if (SB[tapNo] == -1)
//             {
//                 SB[tapNo] = studentId;
//                 SBindex = tapNo;
//             }
//             else if (SB[tapNo - 1] == -1)
//             {
//                 SB[tapNo - 1] = studentId;
//                 SBindex = tapNo-1;
//             }
//             pthread_mutex_unlock(&lock1);
//         }

//         //if(SBindex==-1)sleep(9);
//     }

//     sleep(9);
//     //completionTime[studentId]=currtime+18;
//     //sleep(1);
//     pthread_mutex_lock(&lock1);
//     SB[SBindex]=-1;
//     pthread_mutex_unlock(&lock1);
//     sleep(9);
//     pthread_mutex_lock(&lock1);
//     tap[tapNo] = -1;
//     pthread_mutex_lock(&lock1);
//    // pthread_mutex_unlock(&lock1);
//     clock_t endTime = clock();
//     double readings = (double)(endTime - ThreadcurrTime) / CLOCKS_PER_SEC*1.0;
//     //pthread_mutex_lock(&lock1);
//     //completionTime[studentId]= readings;
//     cout<<"ID: "<<studentId<<" Read: "<<readings<<"\n";
//     //pthread_mutex_unlock(&lock1);
//     //pthread_exit(NULL);
//     return 0;
// }

// void tapAssignment(int n, int m, vector<pthread_t> &Nstudens)
// {
//     for (int i = 0; i < n; i++)
//     {
//         tap.push_back(-1);
//     }
//     for (int i = 0; i < n - 1; i++)
//     {
//         SB.push_back(-1);
//     }
//     currTime = clock();
//     while (!waitingQueue.empty())
//     {
//         for (int i = 0; i < n; i++)
//         {
//             if (tap[i] == -1)
//             {
//                 //cout<<"Hello"<<endl;
//                 tap[i] = waitingQueue.front();
//                 int st = waitingQueue.front();
//                 waitingQueue.pop();
//                 tapAssig[st] = i;
//                 //cout<<"Hello"<<endl;
//                 pthread_create(&Nstudens[i], NULL, task, &studentId[st]);

//                 break;
//             }
//         }
//     }
//     for (int i = 0; i < n; i++)
//     {
//         pthread_join(Nstudens[i], NULL);
        
//     }
     
// }

// int main(int argc, char **argv)
// {
//     int n = stoi(argv[1]);
//     int m = stoi(argv[2]);

//     if (pthread_mutex_init(&lock1, NULL) != 0)
//     {
//         printf("\n mutex init has failed\n");
//         return 1;
//     }

//     for (int i = 0; i < m; i++)
//     {
//         state.push_back(waitingForTap);
//         completionTime.push_back(0);
//         studentId.push_back(i);
//         waitingQueue.push(i);
//     }
//     vector<pthread_t> Nstudents(n);
//     tapAssignment(n, m, Nstudents);
//     pthread_mutex_destroy(&lock1);
// }

