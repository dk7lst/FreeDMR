#include <stdlib.h>
#include <stdio.h>
#include "../../lib/platform/platform.h"
#include "../../lib/data/opt.h"
#include "../../lib/homebrew/homebrewclient.h"

enum {
  P_ServerHost,
  P_ServerPort,
  P_ServerPasswd,
  P_SimMode,
  P_DebugMode,
  //P_LogFile,
  P_LogLevel,
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
};

int main(int argc, char *argv[]) {
  Opt opt;
  opt.registerOpt(P_ServerHost, "-sh", false, true, "specify homebrew server host (default: 127.0.0.1)", "127.0.0.1");
  opt.registerOpt(P_ServerPort, "-sp", false, true, "specify homebrew server port (default: 62030)", "62030");
  opt.registerOpt(P_ServerPasswd, "-pw", false, true, "specify homebrew server password");
  opt.registerOpt(P_SimMode, "-s", false, false, "simulate without real network connection");
  opt.registerOpt(P_LogLevel, "-ll", false, true, "log level");
  opt.registerOpt(P_DebugMode, "-debug", false, false, "set debug mode");
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
  if(!opt.parse(argc, argv)) {
    opt.printHelp();
    return 1;
  }
  
  bool bDebug = opt.get(P_DebugMode);
  bool bSimMode = opt.get(P_SimMode);

  if(bDebug) opt.dump();
  
  HomebrewClient client;
  client.setLogFile(stdout, opt.getInt(P_LogLevel));
  client.setSimulationMode(bSimMode);

  client.setRptId(opt.getInt(P_RptId));
  client.setRptCallsign(opt.getString(P_Callsign));
  client.setRxFrequency_Hz(opt.getInt(P_FreqRx));
  client.setTxFrequency_Hz(opt.getInt(P_FreqTx));
  client.setColorCode(opt.getInt(P_ColorCode));
  client.setLatitude(opt.getDouble(P_RptLat));
  client.setLongitude(opt.getDouble(P_RptLon));
  client.setHeight_m(opt.getInt(P_RptHeight));
  client.setLocation(opt.getString(P_RptLocation));
  client.setDescription(opt.getString(P_RptDescr));
  client.setHomepageURL(opt.getString(P_RptURL));
  client.setSoftwareId(opt.getString(P_SoftwareId));
  client.setPackageId(opt.getString(P_PackageId));

  client.open(opt.getString(P_ServerHost).c_str(), opt.getString(P_ServerPasswd).c_str(), opt.getInt(P_ServerPort));
  client.close();
  return 0;
}
