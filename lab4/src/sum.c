#include "sum.h"

int Sum(const struct SumArgs *args) {
  int sum = 0;
  for (int i = args->begin; i < args->end; i++)
  {
      sum+=args->array[i];
  }
  // TODO: your code here 
  return sum;
}