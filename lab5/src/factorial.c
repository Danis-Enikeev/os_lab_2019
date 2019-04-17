#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <getopt.h>

#include <pthread.h>

struct FactArgs {
  int begin;
  int end;
  int mod;
};

void *ThreadSum(void *args) {
  struct FactArgs *fact_args = (struct FactArgs *)args;
  return (void *)(size_t)Factorial(fact_args);
}
unsigned long int fullFactorial = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int Factorial (struct FactArgs *args)
{
  int factorial = 1;
  
  for (int i = args->begin; i < args->end; i++)
  {
      factorial = (factorial * i) % args->mod;
  }
  printf("factorial part: %d\n", factorial);
  pthread_mutex_lock(&mut);
  fullFactorial = (fullFactorial * factorial) % args->mod;
  pthread_mutex_unlock(&mut);
}
int main(int argc, char **argv) {

  uint32_t k = 0;
  uint32_t pnum = 0;
  uint32_t mod = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            if (k <= 0) {
                printf("factorial argument (k) is a positive number\n");
                return 1;
            }
            break;
          case 1:
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum is a positive number\n");
                return 1;
            }
            break;
          case 2:
            mod = atoi(optarg);
            if (mod <= 0) {
                printf("mod is a positive number\n");
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
    
  if (k == 0 || pnum== 0 || mod== 0) {
    printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }
  pthread_t threads[pnum];
 
  struct FactArgs args[pnum];
  int part = k/pnum;
  for (int i=0; i<pnum; i++){
    args[i].begin = i*part+1;
    args[i].end = (i == pnum-1) ? k+1 : (i + 1) * part+1;
    args[i].mod = mod;
  }
  struct timeval start_time;
  gettimeofday(&start_time, NULL);  
  for (uint32_t i = 0; i < pnum; i++) {
    
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }
  for (uint32_t i = 0; i < pnum; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    
  }
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  printf("Elapsed time: %fms\n", elapsed_time);

  printf("Total: %d\n", fullFactorial);
  return 0;
}
