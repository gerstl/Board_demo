#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <err.h>

#define READ_CMD  (0x0 << 31)
#define WRITE_CMD (0x1 << 31)

volatile int det_int = 0;

// signal handler for receiving events from hardware driver
void sighandler(int signo)
{
  if(signo==SIGIO)
    {
      det_int++;
      printf("\nInterrupt detected\n");
    }
  
  return;
}


int main(int argc, char * argv[]) 
{
  unsigned long trig, val_A, val_B, result;
  unsigned long iie, gie;
  struct sigaction action;
  int fd;

  //Ensure proper usage
  if(argc < 3)
  {
    printf("Usage: %s [valA] [valB]\n",argv[0]);
    return -1;
  }

  // install signal handler
  sigemptyset(&action.sa_mask);
  sigaddset(&action.sa_mask, SIGIO);

  action.sa_handler = sighandler;
  action.sa_flags=0;

  sigaction(SIGIO, &action, NULL);

  // open hardware device (driver)
  fd=open("/dev/fpga", O_RDWR);
  if(fd < 0)
  {

      printf("Unable to open /dev/fpga.  Ensure it exists!\n");
      return -1;
  }
  fcntl(fd, F_SETOWN, getpid());
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_ASYNC);

  // enable FPGA interrupts (global and IP)
  if(ioctl(fd, READ_CMD + 0x1, &gie)) err(1, "Getting GIE");
  gie = gie | 0x00000001;
  if(ioctl(fd, WRITE_CMD + 0x1, &gie)) err(1, "Setting GIE");

  iie = 0x1;
  if(ioctl(fd, WRITE_CMD + 0x2, &iie)) err(1, "Setting IIE");

  // perform C += A*B;
  val_A = atol(argv[1]);
  val_B = atol(argv[2]);

  // write A
  if(ioctl(fd, WRITE_CMD + 0x4, &val_A)) err(1, "Writing A");
  printf("A is %lu\n", val_A);
  
  // write B
  if(ioctl(fd, WRITE_CMD + 0x6, &val_B)) err(1, "Writing B");
  printf("B is %lu\n", val_B);
  
  // trigger MAC operation
  trig = 0x1;
  if(ioctl(fd, WRITE_CMD, &trig)) err(1, "Trigger");

  // wait for interrupt
  while(!det_int) continue;

  // read result
  if(ioctl(fd, READ_CMD + 0x8, &result)) err(1, "Read result");

  printf("C += A*B is %lu\n", result);

  //In the end, close the device driver
  close(fd);

  return 0;
}
