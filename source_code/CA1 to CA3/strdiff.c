#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_ARGUMENT_LENGTH 15

void
exit_len()
{
  printf(2, "Error: Words must be 1 to 15 characters.\n");
  exit();
}

int
main(int argc, char *argv[])
{
  int i, len1, len2, max_len, fd;
  char emp[15];
  if(argc != 3){
    printf(2, "Usage: strdiff <word1> <word2>\n");
    exit();
  }
    
  len1 = strlen(argv[1]);
  len2 = strlen(argv[2]);
  if (len1 > MAX_ARGUMENT_LENGTH || len2 > MAX_ARGUMENT_LENGTH)
    exit_len();
  
  max_len = (len1 > len2)? len1 : len2;
  
  char result[max_len + 1];
  for(i = 0; i < max_len; i++)
    if(i + 1 > len1)
      result[i] = '1';
    
    else if(i + 1 > len2)
      result[i] = '0';
    
    else if (argv[1][i] >= argv[2][i])
      result[i] = '0';
    
    else if (argv[1][i] < argv[2][i])
      result[i] = '1';
    
  if((fd = open("strdiff_result.txt", O_CREATE | O_RDWR)) >= 0){
    write(fd, result, strlen(result));
    write(fd, emp, MAX_ARGUMENT_LENGTH - strlen(result));
    write(fd, (char *)"\n", 1);
    close(fd);
  } 
  else
      printf(2, "Error: create strdiff_result.txt file failed.\n");
  
  exit();
}