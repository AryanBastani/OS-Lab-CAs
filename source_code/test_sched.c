#include "types.h"
#include "user.h"

inline static void show_info();
inline static void set_queue(int pid, int new_queue);

inline static void set_proc_bjf(int pid, int priority_ratio, int arrival_time_ratio,
                                int executed_cycle_ratio, int process_size_ratio);

inline static void set_global_bjf(int priority_ratio, int arrival_time_ratio,
                                  int executed_cycle_ratio, int process_size_ratio);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    exit();
  }
  if (!strcmp(argv[1], "show_info"))
    show_info();
  else if (!strcmp(argv[1], "set_queue"))
  {
    if (argc < 4)
    {
      exit();
    }
    set_queue(atoi(argv[2]), atoi(argv[3]));
  }
  else if (!strcmp(argv[1], "set_proc_bjf"))
  {
    if (argc < 6)
    {
      exit();
    }
    set_proc_bjf(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
  }
  else if (!strcmp(argv[1], "set_global_bjf"))
  {
    if (argc < 6)
    {
      exit();
    }
    set_global_bjf(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
  }

  exit();
}

inline static void show_info()
{
  show_procs_info();
}

inline static void set_queue(int pid, int new_queue)
{
  if (pid < 1)
  {
    printf(1, "ERROR: invalid pid\n");
    return;
  }
  if (new_queue < 1 || new_queue > 3)
  {
    printf(1, "ERROR: invalid queue\n");
    return;
  }
  int res = change_scheduling_queue(pid, new_queue);
  if (res < 0)
    printf(1, "ERROR: changing queue failed\n");
  else
    printf(1, "Process queue changed successfully\n");
}

inline static void set_proc_bjf(int pid, int priority_ratio, int arrival_time_ratio,
                                int executed_cycle_ratio, int process_size_ratio)
{
  if (pid < 1)
  {
    printf(1, "ERROR: invalid pid\n");
    return;
  }
  if (priority_ratio < 0 || arrival_time_ratio < 0 || executed_cycle_ratio < 0 || process_size_ratio < 0)
  {
    printf(1, "ERROR: invalid ratios\n");
    return;
  }
  int res = set_proc_bjf_params(pid, priority_ratio, arrival_time_ratio, executed_cycle_ratio, process_size_ratio);
  if (res < 0)
    printf(1, "ERROR: setting BJF params failed\n");
  else
    printf(1, "BJF params set successfully\n");
}

inline static void set_global_bjf(int priority_ratio, int arrival_time_ratio,
                                  int executed_cycle_ratio, int process_size_ratio)
{
  if (priority_ratio < 0 || arrival_time_ratio < 0 || executed_cycle_ratio < 0 || process_size_ratio < 0)
  {
    printf(1, "ERROR: invalid ratios\n");
    return;
  }
  set_global_bjf_params(priority_ratio, arrival_time_ratio, executed_cycle_ratio, process_size_ratio);
  printf(1, "global BJF params set successfully\n");
}