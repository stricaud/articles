#include <stdlib.h>
#include <unistd.h>

void attendre(void)
{
  sleep(1);
}

int main(void)
{
  void *p = malloc(42);

  attendre();

  free(p);
  
  return 42;
}
