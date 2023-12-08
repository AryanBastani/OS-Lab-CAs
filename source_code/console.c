// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void consputc(int);

static int panicked = 0;

static int distance = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
#define CONTROL_B 0x101
#define CONTROL_F 0x102
#define CONTROL_L 0x103
#define CONTROL_U 0x104
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

#define INPUT_BUF 128
#define COMMAND_BUF 10
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
  uint c; // Cursor index
} input;

static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);
  

  if(c == '\n'){ 
    pos += distance;
    distance = 0;
    input.c = input.e;
    pos += 80 - pos%80;
  }
    
  else if(c == BACKSPACE || c == CONTROL_U){
    if(c == CONTROL_U){
      pos += distance;
      distance = 0;
    }
    if(pos > 0){
      if(distance > 0){
          ushort per_elmnt, curr_elmnt = crt[pos + distance];
          for(int i = pos + distance; i >= pos; i--){
            per_elmnt = crt[i - 1];
            crt[i - 1] = curr_elmnt;
            curr_elmnt = per_elmnt;
          }
      }
      pos--;
    }
  } 
  else if(c == CONTROL_B && pos > 0 &&
    input.buf[(input.c-1) % INPUT_BUF] != '\n'){
        if(input.c != input.w){
          input.c--;
          pos--;
          distance++;
        }
  }
  else if(c == CONTROL_F){
    input.c++;
    pos++;
    distance--;
  }
  else if(c == CONTROL_L){
    pos += distance;
    distance = 0;
    while(pos > 0)
      crt[pos--] = ' ' | 0x0700;
    crt[0] = '$' | 0x0700;
    crt[1] = ' ' | 0x0700;
    pos = 2;
  }
  else{
    if(distance > 0){
      ushort next_elmnt, curr_elmnt = crt[pos];
        for(int i = pos; i <= pos + distance; i++){
          next_elmnt = crt[i + 1];
          crt[i + 1] = curr_elmnt;
          curr_elmnt = next_elmnt;
        }
    }
    crt[pos++] = (c&0xff) | 0x0700;  // black on white
  }
 

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);


  if((c != CONTROL_B && c != CONTROL_F) && distance == 0)
    crt[pos] = ' ' | 0x0700;
  else if(c != CONTROL_B && distance > 0)
    crt[pos + distance] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE || c == CONTROL_U){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);   
  cgaputc(c);
}

char cmds[COMMAND_BUF][INPUT_BUF];
int cmd_idx = 0;
int cmd_pointer = 0;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;
  int i, n, j;
  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      input.c = input.e;
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        input.c--;
        consputc(CONTROL_U);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.c != input.w && input.buf[(input.c-1) % INPUT_BUF] != '\n'){
        if(input.c != input.e){
          ushort per_elmnt, curr_elmnt = input.buf[input.e % INPUT_BUF];
          for(int i = input.e; i >= input.c; i--){
            per_elmnt = input.buf[(i - 1) % INPUT_BUF];
            input.buf[(i - 1) % INPUT_BUF] = curr_elmnt;
            curr_elmnt = per_elmnt;
          }
        }
        input.c--;
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('L'):
      memset(input.buf, 0, 128);
      input.e = input.w;
      input.c = input.e;
      cgaputc(CONTROL_L);
      break;
    case C('B'):
      if(input.c != input.w &&
            input.buf[(input.c-1) % INPUT_BUF] != '\n'){
              consputc(CONTROL_B);
            }
      break;
    case C('F'):
      if(input.c != input.e &&
        input.buf[(input.e + 1) % INPUT_BUF] != '\n'){
              consputc(CONTROL_F);
            }
      break;
    case 0xE2: // UP key
      if(cmd_pointer > 0){
        cmd_pointer--;
        while(input.e != input.w && input.buf[(input.e-1) % INPUT_BUF] != '\n'){
          input.e--;
          input.c--;
          consputc(BACKSPACE);
        }
        input.w = input.e;
        for(i = 0; cmds[cmd_pointer][i] != '\0'; i++){
          input.buf[(input.e) % INPUT_BUF] = cmds[cmd_pointer][i];
          consputc(cmds[cmd_pointer][i] & 0xff);
          input.e++;
          input.c++;
        }
      }
    break;
    case 0xE3: // DOWN key
      if(cmd_pointer != cmd_idx){
        cmd_pointer++;
        while(input.e != input.w && input.buf[(input.e-1) % INPUT_BUF] != '\n'){
          input.e--;
          input.c++;
          consputc(BACKSPACE);
        }
        input.w = input.e;
        for(i = 0; cmds[cmd_pointer][i] != '\0'; i++){
          input.buf[(input.e) % INPUT_BUF] = cmds[cmd_pointer][i];
          consputc(cmds[cmd_pointer][i] & 0xff);
          input.e++;
          input.c++;
        }
      }
    break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        if(input.e == input.c || c == '\n'){
          input.buf[input.e++ % INPUT_BUF] = c;
          input.c++;
        }
        else{
          ushort next_elmnt, curr_elmnt = input.buf[input.c % INPUT_BUF];
          input.e++;
          for(int i = input.c; i < input.e; i++){
            next_elmnt = input.buf[(i + 1) % INPUT_BUF];
            input.buf[(i + 1) % INPUT_BUF] = curr_elmnt;
            curr_elmnt = next_elmnt;
          }
          input.buf[input.c++ % INPUT_BUF] = c;
        }

        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          if(c == '\n'){
            if(cmd_idx == COMMAND_BUF){  // clear all elements before shift all commands up
              for(i = 0; i < COMMAND_BUF - 1; i++){
                for(n = 0; n < INPUT_BUF; n++)
                  cmds[i][n] = '\0';
                for(j = 0; cmds[i + 1][j] != '\0'; j++)
                  cmds[i][j] = cmds[i + 1][j];
              }
              for(n = 0; n < INPUT_BUF; n++)
                cmds[COMMAND_BUF - 1][n] = '\0';
              cmd_idx--;
            }
              
            for(i = input.w; i < input.e - 1; i++)  // insert new command
              cmds[cmd_idx][i - input.w] = input.buf[i % INPUT_BUF];
            
            cmd_idx = (cmd_idx == COMMAND_BUF)? COMMAND_BUF : cmd_idx + 1;
            cmd_pointer = cmd_idx;
          }
          input.w = input.e;
          wakeup(&input.r);
        }
        // input.end++;
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

