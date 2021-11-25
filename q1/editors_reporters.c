/****
  * File containing code to simulate the editors - reporters problem
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

#define MAX_REPORTS 100

/*
 * report_list_lock acts as a global lock,
 * used to gain access to entire report_status and report_lock array.
 * report_lock array contains individual locks to each report.
 * report_count contains the total number of reports (always below MAX_REPORTS).
 */
sem_t report_list_lock;
sem_t report_lock[MAX_REPORTS];
int report_status[MAX_REPORTS];
int report_count;

/*
 * Generate a random number and
 * convert it to integer in range
 * start <= X <= end.
 */
int generate_random_number(int start, int end){
  return random() % ( end - start + 1 ) + start;
}

void* begin(void* arg){
  int id = (uintptr_t)arg;
  // Initialise list to hold list of reports accepted by current editor
  int accepted_list[report_count];
  for(int i=0;i<report_count;++i){
    accepted_list[i] = 0;
  }
  // Initialise list to hold list of reports rejected by current editor
  int rejected_list[report_count];
  for(int i=0;i<report_count;++i){
    rejected_list[i] = 0;
  }
  while(1){
    // Set exit flag to true. i.e., if 0 options are available, editor will exit
    int exit_flag = 1;
    // Initialise list to hold available options
    int options[report_count];
    int options_count = 0;
    sem_wait(&report_list_lock);
    for(int i=0;i<report_count;++i){
      if(rejected_list[i] == 0 && report_status[i] == 1){
        // Try to get access to report
        int res = sem_trywait(&report_lock[i]);
        if(res != 0 && errno == EAGAIN){  // Able to access erport, add it to options
          exit_flag = 0;
        }else if(res == 0){ // Not able to access report, set exit flag to false
          options[options_count++] = i;
          sem_post(&report_lock[i]);
        }
      }
    }
    // printf("considering options: ");
    // for(int i=0;i<options_count;++i){
    //   printf("%d ", options[i]);
    // }
    // printf("\n");
    if(exit_flag == 1 && options_count == 0){
      // Release lock and exit loop
      sem_post(&report_list_lock);
      break;
    }
    
    if(options_count > 0){
      int selected = options[generate_random_number(0, options_count-1)];
      // printf("selected report %d to read\n", selected);
      sem_wait(&report_lock[selected]); // Get lock to selected report
      sem_post(&report_list_lock);  // Release lock to entire report list
      sleep(1); // Read report
      if(generate_random_number(0, 99)%2 == 1){
        // printf("Accepted report: %d\n", selected);
        report_status[selected] = 0;  // Accept article
        accepted_list[selected] = 1;
      }else{
        // printf("Rejected report: %d\n", selected);
        rejected_list[selected] = 1;
      }
      sem_post(&report_lock[selected]); // Release lock to selected report
    }else{
      sem_post(&report_list_lock);  // Release lock to entire report list
    }
  }

  char file_name[100];
  bzero(file_name, 100);
  char id_str[10];
  bzero(id_str, 10);
  sprintf(id_str, "%d", id);
  strcpy(file_name, "editor_");
  strcat(file_name, id_str);
  strcat(file_name, "_logs.txt");
  FILE* logger = fopen(file_name, "w");

  fprintf(logger, "Accepted items: ");
  for(int i=0;i<report_count;++i){
    if(accepted_list[i] == 1){
      fprintf(logger, "%d ", i);
    }
  }
  fprintf(logger, "\n");
  fprintf(logger, "Rejected items: ");
  for(int i=0;i<report_count;++i){
    if(rejected_list[i] == 1){
      fprintf(logger, "%d ", i);
    }
  }
  fprintf(logger, "\n");

  fclose(logger);
  // printf("Exiting...\n");
}

int main(int argc, char* argv[]){
  if(argc != 3){
    printf("Usage: %s <no of editors> <no of reports>\n", argv[0]);
    return 1;
  }

  // Seed random number generator
  srandom(time(NULL));

  int editor_count = atoi(argv[1]);
  pthread_t editor_list[editor_count];

  // Initialise report related data structures
  report_count = atoi(argv[2]);
  for(int i=0;i<report_count;++i){
    // report_list[i].id = i;
    report_status[i] = 1;
  }

  // Initialise locks
  sem_init(&report_list_lock, 0, 1);
  for(int i=0;i<report_count;++i){
    sem_init(&report_lock[i], 0, 1);
  }

  // Start each editor as a separate thread
  for(int i=0;i<editor_count;++i){
    pthread_create(&editor_list[i], NULL, begin, (void*)(uintptr_t)i);
  }
  // Wait for threads to finish execution
  for(int i=0;i<editor_count;++i){
    pthread_join(editor_list[i], NULL);
  }

  // Destroy locks
  for(int i=0;i<report_count;++i){
    sem_destroy(&report_lock[i]);
  }
  sem_destroy(&report_list_lock);

  return 0;
}
