/****
  * File containing multi threaded implementation of merge sort
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
#include <pthread.h>

// Set namespace
using namespace std;

struct thread_info {
  vector<int> arr;
  int start;
  int end;
};

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

void* merge_sort(void* arg){
  struct thread_info* info = (struct thread_info*)arg;
  int start = info->start;
  int end = info->end;
  // printf("start: %d, end: %d\n", start, end);
  if(end - start <= 5){
    // printf("here start\n");
    // Perform selection sort on array
    int size = end - start;
    for(int i=0;i<size-1;++i){
      int pos = start + i;
      int min = info->arr[pos];
      for(int j=i;j<size;++j){
        if(info->arr[start+j] < min){
          pos = start+j;
          min = info->arr[pos];
        }
      }
      int temp = info->arr[start + i];
      info->arr[start + i] = info->arr[pos];
      info->arr[pos] = temp;
    }
    // sort(arr.begin()+start, arr.begin()+end, comp);
    // printf("here end\n");
  }else{
    int mid = (start+end)/2;
    struct thread_info* left_half = new thread_info;
    left_half->arr = info->arr;
    left_half->start = start;
    left_half->end = mid;
    struct thread_info* right_half = new thread_info;
    right_half->arr = info->arr;
    right_half->start = mid;
    right_half->end = end;

    pthread_t left_thread;
    pthread_t right_thread;
    int res1 = pthread_create(&left_thread, NULL, merge_sort, (void*)left_half);
    int res2 = pthread_create(&right_thread, NULL, merge_sort, (void*)right_half);
    pthread_join(left_thread, NULL);
    pthread_join(right_thread, NULL);
    // merge_sort(left_half);
    // merge_sort(right_half);
    // vector<string> left_half = merge_sort(arr, start, mid);
    // vector<string> right_half = merge_sort(arr, mid, end);

    int left_counter = start;
    int right_counter = mid;
    int counter = start;
    // printf("start of merge \n");
    while(left_counter < mid && right_counter < end){
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
    // printf("end of merge \n");
    //
    // printf("start of left merge\n");
    while(left_counter < mid){
      info->arr[counter] = left_half->arr[left_counter];
      ++counter;
      ++left_counter;
    }
    // printf("end of left merge\n");
    //
    // printf("start of right merge\n");
    while(right_counter < end){
      info->arr[counter] = right_half->arr[right_counter];
      ++counter;
      ++right_counter;
    }
    // printf("end of right merge\n");
  }
  // return arr;
}

int main(){
  srandom(time(NULL));

  int no_of_readings = 10;
  double readings[no_of_readings];

  for(int i=0;i<no_of_readings;++i){
    vector<int> arr;

    for(int i=0;i<100;++i){
      arr.push_back(generate_random_number(0, 9999));
    }

    struct thread_info* info = new thread_info;
    info->arr = arr;
    info->start = 0;
    info->end = arr.size();
    pthread_t main_thread;
    clock_t begin = clock();
    int res = pthread_create(&main_thread, NULL, merge_sort, (void*)info);
    pthread_join(main_thread, NULL);
    clock_t end = clock();
    readings[i] = (double)(end - begin) / CLOCKS_PER_SEC;
  }

  double average = 0;

  for(int i=0;i<no_of_readings;++i){
    average += readings[i]/no_of_readings;
  }

  printf("%lf\n", average);

  // vector<string> sorted = info->arr;

  // printf("Sorted: \n");
  // for(int i=0;i<info->arr.size();++i){
  //   printf("%d ", info->arr[i]);
  // }
  // printf("\n");
}
