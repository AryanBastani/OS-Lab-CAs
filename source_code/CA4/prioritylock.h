#define LOG_NAME "pr_log"
#define MSG_MAX 100
#define NUM_MAX 10
#define INIT_MSG "Initialiaing the priotirylock!\n"
#define ACQUIRE_MSG "is acquiring the lock"
#define RELEASE_MSG "is releasing the priority key"

struct prioritylock 
{
    char* name;          // lock's name
    int pid;             // this is for the process is holding the lock
    struct spinlock lk;  // lock for spinlock
    uint locked;         // is lock holding?
    int processes[NPROC];// pid of members of queue
    int num_of_procs;    // this is a counter for queue members
};
