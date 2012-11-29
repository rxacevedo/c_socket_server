#include <pthread.h>
#include <stdio.h>
int sharedx = 0;
void *threadFunc(void *arg)
{
  printf(" %d %s", sharedx, (char*)arg);
  sharedx++;
}

int main(void)
{
  pthread_t pthreads[10];
  int i = 0;
  for (i; i<10; i++)
  {
    pthread_create(&pthreads[i], NULL, threadFunc, "processing...");
  }
}

