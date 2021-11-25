/****
  * File containing single threaded implementation of merge sort
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

// Set namespace
using namespace std;

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

vector<int> merge_sort(vector<int> arr, int start, int end){
  // Check array size
  if(end - start <= 5){
    // Perform selection sort on array
    int size = end - start;
    for(int i=0;i<size-1;++i){
      int pos = start + i;
      int min = arr[pos];
      for(int j=i;j<size;++j){
        if(arr[start+j] < min){
          pos = start+j;
          min = arr[pos];
        }
      }
      int temp = arr[start + i];
      arr[start + i] = arr[pos];
      arr[pos] = temp;
    }
  }else{
    int mid = (start+end)/2;
    vector<int> left_half = merge_sort(arr, start, mid);
    vector<int> right_half = merge_sort(arr, mid, end);

    int left_counter = start;
    int right_counter = mid;
    int counter = start;
    while(left_counter < mid && right_counter < end){
      if(left_half[left_counter] < right_half[right_counter]){
        arr[counter] = left_half[left_counter];
        ++counter;
        ++left_counter;
      }else{
        arr[counter] = right_half[right_counter];
        ++counter;
        ++right_counter;
      }
    }

    while(left_counter < mid){
      arr[counter] = left_half[left_counter];
      ++counter;
      ++left_counter;
    }

    while(right_counter < end){
      arr[counter] = right_half[right_counter];
      ++counter;
      ++right_counter;
    }
  }
  return arr;
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

    clock_t begin = clock();
    vector<int> sorted = merge_sort(arr, 0, arr.size());
    clock_t end = clock();
    readings[i] = (double)(end - begin) / CLOCKS_PER_SEC;
  }

  double average = 0;

  for(int i=0;i<no_of_readings;++i){
    average += readings[i]/no_of_readings;
  }

  printf("%lf\n", average);

  // printf("Sorted: \n");
  // for(int i=0;i<sorted.size();++i){
  //   printf("%d ", sorted[i]);
  // }
  // printf("\n");
}
