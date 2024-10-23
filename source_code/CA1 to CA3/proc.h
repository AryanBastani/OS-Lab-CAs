// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.

int get_uncle_count(int pid);
int get_process_lifetime(int pid);

struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum schedulequeue { UNSET, ROUND_ROBIN, LCFS, BJF };
#define MAX_AGING_LIMIT 8000
#define BJF_PRIORITY 3

struct bjfinfo {
  int priority;
  float priority_ratio;
  int arrival_time;
  float arrival_time_ratio;
  float executed_cycle;
  float executed_cycle_ratio;
  int process_size;
  float process_size_ratio;
};

struct scheduleinfo {
  enum schedulequeue queue; // Process's queue
  struct bjfinfo bjf;       // BJF scheduling information
  int last_run;             // Last time process was running
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
// Per-process state
struct proc {
  uint sz;                        // Size of process memory (bytes)
  pde_t* pgdir;                   // Page table
  char *kstack;                   // Bottom of kernel stack for this process
  enum procstate state;           // Process state
  int pid;                        // Process ID
  struct proc *parent;            // Parent process
  struct trapframe *tf;           // Trap frame for current syscall
  struct context *context;        // swtch() here to run process
  void *chan;                     // If non-zero, sleeping on chan
  int killed;                     // If non-zero, have been killed
  struct file *ofile[NOFILE];     // Open files
  struct inode *cwd;              // Current directory
  char name[16];                  // Process name (debugging)
  int creation_time;              // Creation time of the process
  struct scheduleinfo sched_info; // Scheduling information
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
