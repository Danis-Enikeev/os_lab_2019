#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <getopt.h>

#include <pthread.h>
#include "utils.h"
#include "sum.h"


void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  /*
   *  TODO:
   *  threads_num by command line arguments
   *  array_size by command line arguments
   *	seed by command line arguments
   */
  uint32_t seed = 0;
  uint32_t array_size = 0;
  uint32_t thread_num = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"thread_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
                printf("seed is a positive number\n");
                return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
                printf("array_size is a positive number\n");
                return 1;
            }
            break;
          case 2:
            thread_num = atoi(optarg);
            if (thread_num <= 0) {
                printf("thread_num is a positive number\n");
                return 1;
            }
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
        
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }
    
  if (seed == 0 || array_size == 0 || thread_num == 0) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --thread_num \"num\" \n",
           argv[0]);
    return 1;
  }
  pthread_t threads[thread_num];

  /*
   * TODO:
   * your code here
   * Generate array here
   */
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  struct SumArgs args[thread_num];
  int part = array_size/thread_num;
  for (int i=0; i<thread_num; i++){
    args[i].begin = i*part;
    args[i].end = (i == thread_num) ? array_size : (i + 1) * part;
    args[i].array = array;  
  }
  for (uint32_t i = 0; i < thread_num; i++) {
    
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args)) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < thread_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  free(array);
  printf("Total: %d\n", total_sum);
  return 0;
}
