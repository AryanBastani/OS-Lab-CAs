#include "types.h"
#include "user.h"

#define INF 1000000000000

void make_very_long_calculations()
{
  for (int i = 0; i < 6; i++)
  {
    int pid = fork();
    if (pid > 0)
      continue;
    if (pid == 0)
    {
      sleep(5555);
      for (int j = 0; j < i * 123; j++)
      {
        long long num = 1;
        for (long long k = 0; k < INF; k++)
          num = num << 1;
      }
      exit();
    }
  }
}

int main()
{
  make_very_long_calculations();
  while (wait() != -1)
    ;
  exit();
}