/****
  * File containing multi process implementation of merge sort
  *
  */

// Dependencies
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

// Set namespace
using namespace std;

#define MAX_SIZE 100

struct thread_info {
  int arr[MAX_SIZE];
  int start;
  int end;
};

int segment_id;

/*
 * Function to generate a random number.
 * Takes arguments A abd B and returns an integer at random
 * from the range A <= X <= B.
 */
int generate_random_number(int start, int end){
  return random() % ( end - start + 1 ) + start;
}

/*
 * Function to compare 2 strings.
 * Returns true if first argument is smaller than second argument.
 */
int comp(string a, string b){
  if(a.length() < b.length()){
    return 1;
  }else if(a.length() > b.length()){
    return 0;
  }else{
    return a < b;
  }
}

void merge_sort(struct thread_info* info, int start, int end){

  // printf("inside before: \n");
  // for(int i=0;i<MAX_SIZE;++i){
  //   printf("%d ", info->arr[i]);
  // }
  // printf("\n");

  // int start = info->start;
  // int end = info->end;
  // Check array size
  if(end - start <= 5){
    // Perform selection sort on array
    int size = end - start;
    for(int i=0;i<size-1;++i){
      int pos = start + i;
      int min = info->arr[pos];
      for(int j=i;j<size;++j){
        // if(comp(info->arr[start+j], min) == 1){
        if(info->arr[start+j] < min){
          pos = start+j;
          min = info->arr[pos];
        }
      }
      int temp = info->arr[start + i];
      info->arr[start + i] = info->arr[pos];
      info->arr[pos] = temp;
    }

    // printf("After selection sort: \n");
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", info->arr[i]);
    // }
    // printf("\n");
  }else{
    int mid = (start+end)/2;

    // int segment_id1 = shmget(IPC_PRIVATE, sizeof(struct thread_info), IPC_CREAT | 0666);
    // printf("left: %d\n", segment_id1);
    // if(segment_id1 == -1){
    //   printf("Out of memory\n");
    //   exit(1);
    // }

    // struct thread_info* left_half = (struct thread_info*)shmat(segment_id, NULL, 0);
    // for(int i=0;i<MAX_SIZE;++i){
    //   left_half->arr[i] = info->arr[i];
    // }
    // left_half->start = start;
    // left_half->end = mid;

    // int segment_id2 = shmget(IPC_PRIVATE, sizeof(struct thread_info), IPC_CREAT | 0666);
    // printf("right: %d\n", segment_id2);
    // if(segment_id2 == -1){
    //   printf("Out of memory\n");
    //   exit(1);
    // }

    // struct thread_info* right_half = (struct thread_info*)shmat(segment_id, NULL, 0);
    // for(int i=0;i<MAX_SIZE;++i){
    //   right_half->arr[i] = info->arr[i];
    // }
    // right_half->start = mid;
    // right_half->end = end;

    pid_t left_process_id = fork();
    if(left_process_id == 0){
      struct thread_info* l_h = (struct thread_info*)shmat(segment_id, NULL, 0);
      // cout<<"start: "<<start<<"mid: "<<mid<<endl;
      merge_sort(l_h, start, mid);
      // for(int i=0;i<MAX_SIZE;++i){
      //   left_half->arr[i] = l_h->arr[i];
      // }
      shmdt(l_h);
      // printf("left inside: \n");
      // for(int i=0;i<MAX_SIZE;++i){
      //   printf("%s ", left_half->arr[i].c_str());
      // }
      // printf("\n");
      // shmdt(l_h);
      exit(0);
      // printf("left check\n");
    }
    pid_t right_process_id = fork();
    if(right_process_id == 0){
      struct thread_info* r_h = (struct thread_info*)shmat(segment_id, NULL, 0);
      // cout<<"end: "<<end<<"mid: "<<mid<<endl;
      merge_sort(r_h, mid, end);
      // for(int i=0;i<MAX_SIZE;++i){
      //   right_half->arr[i] = r_h->arr[i];
      // }
      shmdt(r_h);
      // printf("right inside: \n");
      // for(int i=0;i<MAX_SIZE;++i){
      //   printf("%s ", right_half->arr[i].c_str());
      // }
      // printf("\n");
      // shmdt(r_h);
      exit(0);
      // printf("right check\n");
    }
    waitpid(left_process_id, NULL, 0);
    waitpid(right_process_id, NULL, 0);

    // left_half = (struct thread_info*)shmat(segment_id1, NULL, 0);
    // right_half = (struct thread_info*)shmat(segment_id2, NULL, 0);

    // printf("after left: \n");
    // // printf("start: %d, end: %d\n", left_half->start, left_half->end);
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", left_half->arr[i]);
    // }
    // printf("\n");

    // printf("after right: \n");
    // // printf("start: %d, end: %d\n", right_half->start, right_half->end);
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", right_half->arr[i]);
    // }
    // printf("\n");

    // printf("inside just after: \n");
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", info->arr[i]);
    // }
    // printf("\n");

    // vector<string> left_half = merge_sort(arr, start, mid);
    // vector<string> right_half = merge_sort(arr, mid, end);

    struct thread_info* left_half = new thread_info;
    for(int i=0;i<MAX_SIZE;++i){
      left_half->arr[i] = info->arr[i];
    }
    left_half->start = start;
    left_half->end = mid;

    struct thread_info* right_half = new thread_info;
    for(int i=0;i<MAX_SIZE;++i){
      right_half->arr[i] = info->arr[i];
    }
    right_half->start = mid;
    right_half->end = end;

    int left_counter = start;
    int right_counter = mid;
    int counter = start;
    while(left_counter < mid && right_counter < end){
      // if(comp(left_half->arr[left_counter], right_half->arr[right_counter]) == 1){
      if(left_half->arr[left_counter] < right_half->arr[right_counter]){
        info->arr[counter] = left_half->arr[left_counter];
        ++counter;
        ++left_counter;
      }else{
        info->arr[counter] = right_half->arr[right_counter];
        ++counter;
        ++right_counter;
      }
    }

    while(left_counter < mid){
      info->arr[counter] = left_half->arr[left_counter];
      ++counter;
      ++left_counter;
    }

    while(right_counter < end){
      info->arr[counter] = right_half->arr[right_counter];
      ++counter;
      ++right_counter;
    }

    // shmdt(left_half);
    // shmdt(right_half);

    // printf("After merge: \n");
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", info->arr[i]);
    // }
    // printf("\n");
  }
  // return arr;
}

int main(){
  srandom(time(NULL));

  int no_of_readings = 10;
  double readings[no_of_readings];

  for(int i=0;i<no_of_readings;++i){
    int arr[MAX_SIZE];

    for(int j=0;j<MAX_SIZE;++j){
      arr[j] = generate_random_number(0, 9999);
    }

    // printf("herherere\n");

    segment_id = shmget(IPC_PRIVATE, sizeof(struct thread_info), IPC_CREAT | 0666);
    // printf("first: %d\n", segment_id);
    // if(segment_id == -1){
    //   printf("out of memory\n");
    //   exit(1);
    // }
    struct thread_info* info = (struct thread_info*)shmat(segment_id, NULL, 0);
    // printf("here1\n");
    for(int i=0;i<MAX_SIZE;++i){
      // printf("here1 %d\n", i);
      info->arr[i] = arr[i];
    }
    info->start = 0;
    info->end = MAX_SIZE;

    // printf("before sort: \n");
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", info->arr[i]);
    // }
    // printf("\n");

    // printf("before\n");
    clock_t begin = clock();
    merge_sort(info, 0, MAX_SIZE);
    clock_t end = clock();
    readings[i] = (double)(end - begin) / CLOCKS_PER_SEC;
    // printf("after sort: \n");
    // for(int i=0;i<MAX_SIZE;++i){
    //   printf("%d ", info->arr[i]);
    // }
    // printf("\n");
    shmdt(info);
    // break;
  }

  double average = 0;

  for(int i=0;i<no_of_readings;++i){
    average += readings[i]/no_of_readings;
  }

  printf("%lf\n", average);

  // vector<string> sorted = merge_sort(arr, 0, arr.size());

  // printf("Sorted: \n");
  // for(int i=0;i<MAX_SIZE;++i){
  //   printf("%d ", info->arr[i]);
  // }
  // printf("\n");
}
