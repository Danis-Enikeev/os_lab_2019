#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"
pid_t currentPID;
void KillChild(int signum){
    if (kill(currentPID, SIGKILL)==0)
        printf ("child process was successfully killed");
    else
        printf ("error while killing a child process");
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
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
            pnum = atoi(optarg);
            if (pnum <= 0) {
                printf("pnum is a positive number\n");
                return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4: 
            timeout = atoi(optarg);
            if (timeout < 0){
                printf("timeout is a positive number\n");
                return 1;
            }
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
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
    
  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  
  int part = array_size/pnum;
  int pipefd[2];
  pipe(pipefd);
  
  struct MinMax minMaxBuff;
  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process
        // parallel somehow
        minMaxBuff = GetMinMax(array, i*part, (i == pnum - 1) ? array_size : (i + 1) * part);
        if (with_files) {
          // use files here
          FILE* outFile = fopen("out.txt", "a");
          fwrite(&minMaxBuff, sizeof(struct MinMax), 1, outFile);
          fclose(outFile);
        } else {
          // use pipe here
          write(pipefd[1], &minMaxBuff, sizeof(struct MinMax));
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }
  while (active_child_processes > 0) {
    // your code here
    
    if (timeout>0){
        ualarm (timeout*1000, 0);
        signal (SIGALRM, KillChild);
    }
    close(pipefd[1]);
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    if (with_files) {
        FILE* outFile = fopen("out.txt", "rb");
        fseek(outFile, i*sizeof(struct MinMax), SEEK_SET);
        fread(&minMaxBuff, sizeof(struct MinMax), 1, outFile);
        //printf("pnum:%i:\tmin:%7i     max:%i\n", i, minMaxBuff.min, minMaxBuff.max);
        fclose(outFile);
    } else {
      read(pipefd[0], &minMaxBuff, sizeof(struct MinMax));
      //printf( "pnum:%i\tmin:%7i     max:%i\n", i, minMaxBuff.min, minMaxBuff.max);
    }

    if (minMaxBuff.min < min_max.min)
        min_max.min = minMaxBuff.min;
    if (minMaxBuff.max > min_max.max) 
        min_max.max = minMaxBuff.max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

 

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);

  fflush(NULL);
  return 0;
}
