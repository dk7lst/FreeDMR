#include <stdlib.h>
#include <stdio.h>
#include "../../lib/platform/platform.h"
#include "../../lib/data/opt.h"
#include "../../lib/socket/socketlib.h"
#include "../../lib/homebrew/homebrewpacket.h"

enum {
  P_ServerHost,
  P_ServerPort,
  P_ServerPasswd,
  P_LocalPort,
  P_SimMode,
  P_DebugMode,
  //P_LogFile,
  P_LogLevel
/*
  P_RptId,
  P_Callsign,
  P_FreqRx,
  P_FreqTx,
  P_ColorCode,
  P_RptLat,
  P_RptLon,
  P_RptHeight,
  P_RptLocation,
  P_RptDescr,
  P_RptURL,
  P_SoftwareId,
  P_PackageId
*/
};

int main(int argc, char *argv[]) {
  Opt opt;
  opt.registerOpt(P_ServerHost, "-sh", false, true, "specify homebrew server host (default: 127.0.0.1)", "127.0.0.1");
  opt.registerOpt(P_ServerPort, "-sp", false, true, "specify homebrew server port (default: 62030)", "62030");
  opt.registerOpt(P_ServerPasswd, "-pw", false, true, "specify homebrew server password");
  opt.registerOpt(P_LocalPort, "-lp", false, true, "specify local port (default: 62030)", "62030");
  opt.registerOpt(P_SimMode, "-s", false, false, "simulate without real network connection");
  opt.registerOpt(P_LogLevel, "-ll", false, true, "log level");
  opt.registerOpt(P_DebugMode, "-debug", false, false, "set debug mode");
/*
  opt.registerOpt(P_RptId, "-id", false, true, "set repeater DMR ID", 0);
  opt.registerOpt(P_Callsign, "-cs", false, true, "set repeater callsign", "NOCALL");
  opt.registerOpt(P_FreqRx, "-frx", false, true, "set rx frequency in Hz");
  opt.registerOpt(P_FreqTx, "-ftx", false, true, "set rx frequency in Hz");
  opt.registerOpt(P_ColorCode, "-cc", false, true, "set DMR color-code (default: 1)", "1");
  opt.registerOpt(P_RptLat, "-lat", false, true, "set repeater latitude");
  opt.registerOpt(P_RptLon, "-lon", false, true, "set repeater longitude");
  opt.registerOpt(P_RptHeight, "-hm", false, true, "set repeater height in meters");
  opt.registerOpt(P_RptLocation, "-loc", false, true, "set repeater location");
  opt.registerOpt(P_RptDescr, "-descr", false, true, "set repeater desxcription");
  opt.registerOpt(P_RptURL, "-hp", false, true, "set repeater homepage URL");
  opt.registerOpt(P_SoftwareId, "-sid", false, true, "set software id");
  opt.registerOpt(P_PackageId, "-pid", false, true, "set package id");
*/
  if(!opt.parse(argc, argv)) {
    opt.printHelp();
    return 1;
  }
  
  bool bDebug = opt.get(P_DebugMode);
  //bool bSimMode = opt.get(P_SimMode);

  if(bDebug) opt.dump();
  
  UDPSocket sock;
  sock.bind(opt.getInt(P_LocalPort));
  
  char rxBuffer[1024], buf[1024];
  while(true) {
    IPAddr fromAddr;
    unsigned uFromAddrLength;
    int iBytes = sock.recvfrom(rxBuffer, sizeof rxBuffer, &fromAddr, &uFromAddrLength);
    printf("Received %d bytes from %s:", iBytes, fromAddr.tostring(buf));
    for(int i = 0; i < iBytes; ++i) printf(" %02X", rxBuffer[i]);
    putchar('\n');
    sock.sendto(&fromAddr, rxBuffer, iBytes);
  }
  sock.close();
  return 0;
}
