#include "print.h"
#include "platform.h"
#include "xs1.h"

on stdcore[0]: port p = XS1_PORT_1A;

on stdcore[0]: in port cnt = XS1_PORT_4A;

void monitor_count(void) {
  int val = 0;
  while (1) {
    cnt when pinsneq(val) :> val;
    printintln(val);
  }
}

int main(){
  printstrln("Hello World\n");
  par {
    while (1) {
      p <: 0;
      p <: 1;
    }
    monitor_count();
  }
}
