#if 0
gcc -s -O2 -o ~/bin/ff-bright ff-bright.c -lm
exit
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char buf[16];
static char chan;
static double param[3];

static void process(int pos) {
  int iv=(buf[pos]<<8)|buf[pos+1];
  double v=((double)iv)/65535.0;
  v=param[1]*pow(v,param[2])+param[0];
  iv=v*65535.0;
  if(iv<0) iv=0;
  if(iv>65535) iv=65535;
  buf[pos]=iv>>8;
  buf[pos+1]=iv;
}

int main(int argc,char**argv) {
  int i;
  i=fread(buf,1,16,stdin);
  i=fwrite(buf,1,16,stdout);
  if(argc>1) {
    chan=0;
    for(i=0;argv[1][i];i++) {
      if(argv[1][i]=='r') chan|=1;
      if(argv[1][i]=='g') chan|=2;
      if(argv[1][i]=='b') chan|=4;
      if(argv[1][i]=='a') chan|=8;
    }
  } else {
    chan=7;
  }
  param[0]=argc>2?strtod(argv[2],0):0.0;
  param[1]=argc>3?strtod(argv[3],0):1.0;
  param[2]=argc>4?strtod(argv[4],0):1.0;
  while(fread(buf,1,8,stdin)) {
    if(chan&1) process(0);
    if(chan&2) process(2);
    if(chan&4) process(4);
    if(chan&8) process(6);
    i=fwrite(buf,1,8,stdout);
  }
  return 0;
}

