#include <stdlib.h>
#include <stdio.h>
#include "../../lib/platform/platform.h"
#include "../../lib/data/opt.h"
#include "../../lib/dv4mini/dv4mini.h"

int main(int argc, char *argv[]) {
  Opt opt;
  opt.registerOpt("-d", false, true, "specify DV4Mini device name", "/dev/ttyACM0");
  opt.registerOpt("-f", false, true, "set frequency in Hz");
  opt.registerOpt("-s", false, false, "simulate without real hardware");
  opt.registerOpt("-debug", false, false, "set debug mode");
  if(!opt.parse(argc, argv)) {
    opt.printHelp();
    return 1;
  }
  
  bool bDebug = opt.get("-debug");
  bool bSimMode = opt.get("-s");

  if(bDebug) opt.dump();

  std::string sDeviceName = opt.getString("-d");

  DV4Mini mini;
  if(bSimMode) {
    sDeviceName = "/dev/null";
    mini.setSimulationMode(true);
  }
  mini.setLogFile(stdout, 0);
  if(!mini.open(sDeviceName.c_str())) {
    perror(sDeviceName.c_str());
    return 1;
  }
  printf("Using device at %s.\n", sDeviceName.c_str());
  for(int i = 0; i < 50; ++i) {
    usleep(100);
    printf("RSSI=%d\n", mini.getRSSI());
  }
  mini.close();
}
