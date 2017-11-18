#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "../../lib/platform/platform.h"
#include "../../lib/data/opt.h"
#include "../../lib/homebrew/homebrewpacket.h"
#include "../../lib/homebrew/homebrewclient.h"

enum {
  P_ServerHost,
  P_ServerPort,
  P_ServerPasswd,
  P_ServerDialect,
  P_SimMode,
  P_DebugMode,
  //P_LogFile,
  P_LogLevel,
  P_RptId,
  P_Callsign,
  P_FreqRx,
  P_FreqTx,
  P_TxPower,
  P_ColorCode,
  P_RptLat,
  P_RptLon,
  P_RptHeight,
  P_RptLocation,
  P_RptDescr,
  P_RptURL,
  P_SoftwareId,
  P_PackageId,
  P_LoopMode,
  P_DstIdOverride,
  P_RecMode,
  P_RecTime,
  P_RecFile
};

void SignalHandler(int signum);

int g_bExitRequest = false;

void SignalHandler(int signum) {
  g_bExitRequest = true;
}

int main(int argc, char *argv[]) {
  Opt opt;
  opt.registerOpt(P_ServerHost, "-sh", false, true, "specify homebrew server host (default: 127.0.0.1)", "127.0.0.1");
  opt.registerOpt(P_ServerPort, "-sp", false, true, "specify homebrew server port (default: 62030)", "62030");
  opt.registerOpt(P_ServerPasswd, "-pw", false, true, "specify homebrew server password");
  opt.registerOpt(P_ServerDialect, "-sd", false, true, "specify homebrew protocol dialect: 0:classic 1:MMDVM (default)", "1");
  opt.registerOpt(P_SimMode, "-s", false, false, "simulate without real network connection");
  opt.registerOpt(P_LogLevel, "-ll", false, true, "log level");
  opt.registerOpt(P_DebugMode, "-debug", false, false, "set debug mode");
  opt.registerOpt(P_RptId, "-id", false, true, "set repeater DMR ID", "1");
  opt.registerOpt(P_Callsign, "-cs", false, true, "set repeater callsign", "NOCALL");
  opt.registerOpt(P_FreqRx, "-frx", false, true, "set rx frequency [Hz]");
  opt.registerOpt(P_FreqTx, "-ftx", false, true, "set tx frequency [Hz]");
  opt.registerOpt(P_TxPower, "-pwr", false, true, "set tx power [dBm]", "10");
  opt.registerOpt(P_ColorCode, "-cc", false, true, "set DMR color-code (default: 1)", "1");
  opt.registerOpt(P_RptLat, "-lat", false, true, "set repeater latitude");
  opt.registerOpt(P_RptLon, "-lon", false, true, "set repeater longitude");
  opt.registerOpt(P_RptHeight, "-hm", false, true, "set repeater height in meters");
  opt.registerOpt(P_RptLocation, "-loc", false, true, "set repeater location");
  opt.registerOpt(P_RptDescr, "-descr", false, true, "set repeater desxcription");
  opt.registerOpt(P_RptURL, "-hp", false, true, "set repeater homepage URL");
  opt.registerOpt(P_SoftwareId, "-sid", false, true, "set software id");
  opt.registerOpt(P_PackageId, "-pid", false, true, "set package id");
  opt.registerOpt(P_LoopMode, "-loop", false, false, "repeat playback until aborted with CTRL+C");
  opt.registerOpt(P_DstIdOverride, "-did", false, true, "override DMR destination ID (positive numbers for private calls, negative numbers for group calls)", "0");
  opt.registerOpt(P_RecMode, "-rec", false, false, "record mode, otherwise play mode");
  opt.registerOpt(P_RecTime, "-rtime", false, true, "recording time [sec]", "10");
  opt.registerOpt(P_RecFile, "-file", false, true, "specify file to load/save DMR data to/from", "data.dmr");
  if(!opt.parse(argc, argv)) {
    opt.printHelp();
    return 1;
  }

  const bool bDebug = opt.get(P_DebugMode);
  const bool bSimMode = opt.get(P_SimMode);
  const bool bLoopMode = opt.get(P_LoopMode);
  const int iDstIdOverride = opt.getInt(P_DstIdOverride);
  const bool bRecMode = opt.get(P_RecMode);
  const int iRecTime = opt.getInt(P_RecTime);
  const std::string sFile = opt.getString(P_RecFile);

  if(bDebug) opt.dump();
  
  HomebrewClient client;
  client.setLogFile(stdout, opt.getInt(P_LogLevel));
  client.setSimulationMode(bSimMode);

  client.setRptId(opt.getInt(P_RptId));
  client.setRptCallsign(opt.getString(P_Callsign));
  client.setRxFrequency_Hz(opt.getInt(P_FreqRx));
  client.setTxFrequency_Hz(opt.getInt(P_FreqTx));
  client.setTxPower_dBm(opt.getInt(P_TxPower));
  client.setColorCode(opt.getInt(P_ColorCode));
  client.setLatitude(opt.getDouble(P_RptLat));
  client.setLongitude(opt.getDouble(P_RptLon));
  client.setHeight_m(opt.getInt(P_RptHeight));
  client.setLocation(opt.getString(P_RptLocation));
  client.setDescription(opt.getString(P_RptDescr));
  client.setHomepageURL(opt.getString(P_RptURL));
  client.setSoftwareId(opt.getString(P_SoftwareId));
  client.setPackageId(opt.getString(P_PackageId));

  signal(SIGINT, SignalHandler); // catch CTRL+C

  puts("Connecting...");
  client.setMaxPacketRxQueueSize(1000);
  if(!client.open(opt.getString(P_ServerHost).c_str(), opt.getString(P_ServerPasswd).c_str(), opt.getInt(P_ServerPort), (HomebrewClient::PROTOCOLDIALECT)opt.getInt(P_ServerDialect))) {
    puts("Connection failed!");
    return 1;
  }

  if(bRecMode) {
    FILE *f = fopen(sFile.c_str(), "wb");
    if(f) {
      for(int i = iRecTime; i > 0 && !g_bExitRequest; --i) {
        printf("Recording %d sec...\n", i);
        sleep(1);
        HomebrewPacket *p;
        while((p = client.getRxPacket()) != NULL && !g_bExitRequest) {
          putchar('.');
          fwrite(p->getRawDataPtr(), p->getRawDataSize(), 1, f);
          delete p;
        }
      }
      fclose(f);
      puts(" done!");
    }
    else perror(sFile.c_str());
  }
  else {
    FILE *f = fopen(sFile.c_str(), "rb");
    if(f) {
      do {
        puts("Playing...");
        rewind(f);
        HomebrewPacket p;
        while(fread((void *)p.getRawDataPtr(), p.getRawDataSize(), 1, f) == 1 && !g_bExitRequest) {
          putchar('.');
          p.setRptId(client.getRptId());
          if(iDstIdOverride) {
            p.setDstId(abs(iDstIdOverride));
            p.setGroupCall(iDstIdOverride < 0);
            //puts(p.toString().c_str());
          }
          client.sendTxPacket(&p);
          usleep(60000); // 3 AMBE-frames per DMR-packet with 20ms of voice data each.
        }
        putchar('\n');
      } while(bLoopMode && !g_bExitRequest);
      fclose(f);
      puts(" done!");
    }
    else perror(sFile.c_str());
  }

  puts("Disconnecting...");
  client.close();
  puts("Exit!");
  return 0;
}
