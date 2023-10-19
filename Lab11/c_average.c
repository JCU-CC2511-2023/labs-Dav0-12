#include "pico/stdlib.h"

uint32_t c_average(uint32_t Q, uint32_t* values) {
  /* TODO - calculate average of the 2^Q unsigned integers in values[] */

  uint64_t average = 0;

  for (int i=0; i < (1 << Q); i++){
  average += (uint64_t)values[i];  
  }
  average = average / (1 << Q);
  if(average > 4294967295){
    printf("Average is greater than 32 bits, will be casted to 32 bits");
  }

  return (uint32_t)average; 
}
