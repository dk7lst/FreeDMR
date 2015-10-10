#include <stdlib.h>
#include <stdio.h>
#include "../../lib/platform/platform.h"
#include "../../lib/data/opt.h"
#include "../../lib/data/filesink.h"
#include "../../lib/dv4mini/dv4mini.h"

enum {
  P_DeviceName,
  P_Frequency,
  P_SimMode,
  P_DebugMode,
  P_WriteRxToFile,
  P_TxFromFile,
  //P_LogFile,
  P_LogLevel
};

int main(int argc, char *argv[]) {
  Opt opt;
  opt.registerOpt(P_DeviceName, "-d", false, true, "specify DV4Mini device name", "/dev/ttyACM0");
  opt.registerOpt(P_Frequency, "-f", false, true, "set frequency in Hz", "430312500");
  opt.registerOpt(P_WriteRxToFile, "-w", false, true, "write received data to file");
  opt.registerOpt(P_TxFromFile, "-tx", false, true, "transmit data from file");
  opt.registerOpt(P_SimMode, "-s", false, false, "simulate without real hardware");
  opt.registerOpt(P_LogLevel, "-ll", false, false, "log level");
  opt.registerOpt(P_DebugMode, "-debug", false, false, "set debug mode");
  if(!opt.parse(argc, argv)) {
    opt.printHelp();
    return 1;
  }
  
  bool bDebug = opt.get(P_DebugMode);
  bool bSimMode = opt.get(P_SimMode);

  if(bDebug) opt.dump();

  std::string sDeviceName = opt.getString(P_DeviceName);
  int iFrequency_Hz = opt.getInt(P_Frequency);

  DV4Mini mini;
  if(bSimMode) {
    sDeviceName = "/dev/null";
    mini.setSimulationMode(true);
  }

  mini.setLogFile(stdout, opt.getInt(P_LogLevel));

  if(!mini.open(sDeviceName.c_str())) {
    perror(sDeviceName.c_str());
    return 1;
  }

  mini.setFrequency(iFrequency_Hz);
  printf("Using device \"%s\" at %d Hz (%.3f kHz).\n", sDeviceName.c_str(), iFrequency_Hz, iFrequency_Hz / 1000.);
  
  mini.setLED(true);
  
  FileSink fsink;
  if(opt.get(P_WriteRxToFile)) {
    const char *pszWriteFileName = opt.getString(P_WriteRxToFile).c_str();
    if(!fsink.open(pszWriteFileName)) {
      perror(pszWriteFileName);
      return 1;
    }
    mini.setRxSink(&fsink);
    printf("Writing received data to file \"%s\".\n", pszWriteFileName);
  }
  
  if(opt.get(P_TxFromFile)) {
    const char *pszReadFileName = opt.getString(P_TxFromFile).c_str();
    FILE *pFile = fopen(pszReadFileName, "r");
    if(!pFile) {
      perror(pszReadFileName);
      return 1;
    }
    printf("Sending file \"%s\"", pszReadFileName);
    BYTE buf[100];
    while(true) {
      putchar('.');
      int iBytes = fread(buf, 1, sizeof buf, pFile);
      if(iBytes <= 0) break;
      mini.transmit(buf, iBytes);
      usleep(10);
    }
    puts("done!\n");
    mini.flush();
    fclose(pFile);
    mini.close();
    return 0;
  }

#if 1
  for(int i = 0; i < 5; ++i) {
    sleep(1);
    printf("RSSI=%d\n", mini.getRSSI());
  }
#else
  for(int i = 0; i < 50; ++i) {
    usleep(100);
    printf("RSSI=%d\n", mini.getRSSI());
  }
#endif
  mini.close();
  sleep(1); // Allow Threads to shut down.
  return 0;
}
